/*Copyright 2009,2010 Alex Graves

This file is part of RNNLIB.

RNNLIB is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RNNLIB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RNNLIB.  If not, see <http://www.gnu.org/licenses/>.*/

#ifndef _INCLUDED_Trainer_h  
#define _INCLUDED_Trainer_h
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <boost/timer.hpp>
#include "Mdrnn.hpp"
#include "Optimiser.hpp"
#include "NetcdfDataset.hpp"
#include "Helpers.hpp"
#include "ConfigFile.hpp"
#include "DatasetErrors.hpp"
#include "Random.hpp"

using namespace std;
extern bool verbose;
#ifdef OP_TRACKING
	extern unsigned long long matrixOps;
#endif

struct Trainer: public DataExporter
{
	//data
	ostream& out;
	Mdrnn* net;
	Optimiser* optimiser;
	ConfigFile& config;
	vector<real_t>& wts;
	vector<real_t>& derivs;
	int epoch;
	const Vector<string>& criteria;
	map<string, real_t>& netErrors;
	const map<string, real_t>& netNormFactors;
	vector<real_t> distortedWeights;
	string task;
	real_t dataFraction;
	int seqsPerWeightUpdate;
	bool batchLearn;
	DataList trainFiles;
	DataList testFiles;
	DataList valFiles;
	DatasetErrors trainErrors;
	DatasetErrors testErrors;
	DatasetErrors valErrors;
	real_t inputNoiseDev;
	real_t weightDistortion;
	bool additiveWeightDistortion;
	bool testDistortions;
	
	//functions
	Trainer(ostream& o, Mdrnn* n, Optimiser* opt, ConfigFile& conf, const string& name = "trainer"):
		DataExporter(name),
		out(o),
		net(n),
		optimiser(opt),
		config(conf),
		wts(WeightContainer::instance().weights),
		derivs(WeightContainer::instance().derivatives),
		epoch(0),
		criteria(net->criteria),
		netErrors(net->errors),
		netNormFactors(net->normFactors),
		task(config.get<string>("task")),
		dataFraction(config.get<real_t>("dataFraction", 1)),
		seqsPerWeightUpdate(config.get<int>("seqsPerWeightUpdate", 1)),
		batchLearn(config.get<bool>("batchLearn", (config.get<string>("optimiser", "steepest") == "rprop") && (seqsPerWeightUpdate == 1))), 
		trainFiles(config.get_list<string>("trainFile"), task, !batchLearn, dataFraction),
		testFiles(config.get_list<string>("testFile"), task, false, dataFraction),
		valFiles(config.get_list<string>("valFile"), task, false, dataFraction),
		inputNoiseDev(config.get<real_t>("inputNoiseDev", 0)),
		weightDistortion(config.get<real_t>("weightDistortion", 0)),  //std deviation of distortions to add to wts during training
		additiveWeightDistortion(weightDistortion ? config.get<bool>("additiveWeightDistortion", true) : false),
		testDistortions(conf.get<bool>("testDistortions", false))
	{
		SAVE(epoch);
	}
	void train(const string& savename)
	{
		check(trainFiles.size(), "no training files loaded");
		int totalEpochs = config.get<int>("totalEpochs", -1);
		int maxTestsNoBest = config.get<int>("maxTestsNoBest", 20);
		PRINT(epoch, out);
		if (totalEpochs >= 0)
		{
			PRINT(totalEpochs, out);			
		}
		if (savename != "")
		{
			PRINT(savename, out);
		}
		PRINT(batchLearn, out);
		PRINT(seqsPerWeightUpdate, out);
		PRINT(maxTestsNoBest, out);
		out << endl;
		print_datasets();
		print_distortions();

		//init filenames
		string bestSaveRoot = savename + ".best";
		string lastSaveFile = savename + ".last.save";
		if (savename != "")
		{
			out << "autosave filename " << lastSaveFile << endl; 
			out << "best save filename root " << bestSaveRoot << endl << endl; 
		}
		config.warn_unused(out);
		
		//init LOOP variables
#ifndef OP_TRACKING
		int numWeights = wts.size();
#endif
		map<string, pair<int, DatasetErrors> > bestTestErrors;
		map<string, pair<int, DatasetErrors> > bestValErrors;
		map<string, pair<int, DatasetErrors> > bestTrainErrors;
		int testsSinceBest = 0;
		
		//LOOP through training data until done
		out << "training..." << endl;
		boost::timer t;
		int initEpoch = epoch;
		int seqsSinceWeightUpdate = 0;
		bool stoppingCriteriaReached = false;
		while (!stoppingCriteriaReached && (epoch < totalEpochs || totalEpochs < 0))
		{
			boost::timer epochT;
			trainErrors.clear();
#ifdef OP_TRACKING
			matrixOps = 0;
#endif
			//run through one epoch, collecting errors and updating weights			
			for (const DataSequence* seq = trainFiles.start(); seq; seq = trainFiles.next_sequence())
			{
				if (verbose)
				{
					out << "data sequence:" << endl;
					out << "file = " << trainFiles.dataset->filename << endl;
					out << "index = " << trainFiles.seqIndex << endl; 
					out << *seq;
				}
				seq = apply_distortions(seq);
				net->train(*seq);
				revert_distortions();
				trainErrors.add_seq_errors(netErrors, netNormFactors);
				if (verbose)
				{
					net->print_output_shape(out);
					out << "errors:" << endl;
					out << netErrors;
				}
				if (!(batchLearn) && (++seqsSinceWeightUpdate >= seqsPerWeightUpdate))
				{
					update_weights();
					seqsSinceWeightUpdate = 0;
				}
				if (verbose)
				{
					out << endl;
				}
			}
			if (batchLearn)
			{
				update_weights();
			}
			trainErrors.normalise();
			
			//print out epoch data
			real_t epochSeconds = epochT.elapsed();
			out << endl << "epoch " << epoch << " took ";
			print_time(epochSeconds, out);
			real_t itsPerSec = (real_t)trainFiles.numTimesteps / epochSeconds;
			out << " (";
			print_time(epochSeconds / (real_t)trainFiles.numSequences, out, true);
			out << "/seq, " << itsPerSec << " its/sec, ";
#ifdef OP_TRACKING
			real_t mWtOpsPerSec = matrixOps / (epochSeconds * 1e6);
			out << mWtOpsPerSec << " MwtOps/sec)" << endl;
#else
			real_t mWtItsPerSec = (itsPerSec * numWeights) / 1e6;
			out << mWtItsPerSec << " MwtIts/sec)" << endl;
#endif
			//print running train errrors
			prt_line(out);
			out << "train errors (running):" << endl;
			out << trainErrors;
			prt_line(out);

			//calculate error test, if required
			if (valFiles.size())
			{
				out << "validation errors:" << endl;  
				out << calculate_errors(valFiles, valErrors);
				prt_line(out);
			}
			if (testFiles.size())
			{
				out << "test errors:" << endl;  
				out << calculate_errors(testFiles, testErrors);
				prt_line(out);
			}
			
			//update epoch BEFORE saves (so training continues one epoch on)
			++epoch;			
			if (savename != "")
			{
				save_data(lastSaveFile, config);
			}
			DatasetErrors currentErrors = valFiles.size() ? valErrors : trainErrors;
			map<string, pair<int, DatasetErrors> >& bestErrors = valFiles.size() ? bestValErrors : bestTrainErrors;
			if (check_for_best(currentErrors, bestErrors, epoch))
			{	
				LOOP(const PSPIDE& p, bestErrors)
				{
					if (p.second.first == epoch)
					{
						const string& s = p.first;
						out << "best network (" << s << ")" << endl;
						if (valFiles.size())
						{
							bestTrainErrors[s] = make_pair(epoch, trainErrors);
						}
						if (testFiles.size())
						{
							bestTestErrors[s] = make_pair(epoch, testErrors);
						}				
						if (savename != "")
						{
							string saveFile = bestSaveRoot + "_" + s + ".save";
							save_data(saveFile, config);
						}
						testsSinceBest = 0;
					}
				}
			}
			//check if training is finished
			else if (maxTestsNoBest > 0 && (++testsSinceBest > maxTestsNoBest))
			{
				out << testsSinceBest << " error tests without best, ending training" << endl;
				stoppingCriteriaReached = true;
			}	
		}

		//autosave, if required
		if (savename != "")
		{
			save_data(lastSaveFile, config);
		}
	
		//print out overall stats
		real_t seconds = t.elapsed();
		out << endl << "training finished, " << epoch << " epochs in total" << endl;
		out << epoch - initEpoch << " epochs in ";
		print_time(seconds, out);
		out << "(this session)" << endl << endl;
		print_best_errors("train", bestTrainErrors);
		out << endl;
		if (valFiles.size())
		{
			print_best_errors("validation", bestValErrors);
			out << endl;
		}
		if (testFiles.size())
		{
			print_best_errors("test", bestTestErrors);
			out << endl;
		}
	}
	bool print_distortions()
	{
		bool distortions = false;
		if (inputNoiseDev != 0)
		{
			out << "adding noise to input data, std dev " << inputNoiseDev << endl << endl; 
			distortions = true;
		}
		if (weightDistortion)
		{
			out << "adding ";
			if (additiveWeightDistortion)
			{
				out << "additive";
			}
			else
			{
				out << "multiplicative";
			}
			out << " noise to weights every sequence, std dev " << weightDistortion << endl << endl; 
			distortions = true;
		}
		return distortions;
	}		
	const DataSequence* apply_distortions(const DataSequence* seq)
	{
		if (inputNoiseDev)
		{
			seq = add_input_noise(seq);
		}
		if (weightDistortion)
		{
			distortedWeights = wts;
			perturb_weights(distortedWeights, weightDistortion, additiveWeightDistortion);
			distortedWeights.swap(wts);
		}
		return seq;
	}
	const void revert_distortions()
	{
		if (weightDistortion)
		{
			distortedWeights.swap(wts);
		}
	}
	DataSequence* add_input_noise(const DataSequence* seq)
	{
		static DataSequence noisySeq;
		noisySeq = *seq;
		LOOP(real_t& f, noisySeq.inputs.data)
		{
			f += Random::normal(inputNoiseDev);
		}
		return &noisySeq;
	}
	void print_datasets() const
	{
		if (trainFiles.size())
		{
			out << "training data:" << endl << trainFiles;
		}
		if (valFiles.size())
		{
			prt_line(out);
			out << "validation data:" << endl << valFiles;
		}
		if (testFiles.size())
		{
			prt_line(out);
			out << "test data:" << endl << testFiles;
		}		
		out << endl;
	}
	void save_data(const string& filename, ConfigFile& conf)
	{
		ofstream fout(filename.c_str());
		if (fout.is_open())
		{
			out << "saving to " << filename << endl;
			config.set_val<bool>("loadWeights", true);
			fout << config << DataExportHandler::instance();
		}
		else
		{
			out << "WARNING trainer unable to save to file " << filename << endl;
		}
	}
	void update_weights()
	{
		optimiser->update_weights();
		WeightContainer::instance().reset_derivs();
	}
	bool check_for_best(const DatasetErrors& currentErrors, map<string, pair<int, DatasetErrors> >& bestErrors, int epoch)
	{
		bool newBest = false;
		LOOP(const PSD& p, currentErrors.errors)
		{
			const string& errName = p.first;
			real_t err = p.second;
			if (in(criteria, errName) && (!in(bestErrors, errName) 
										  || !(in(bestErrors[errName].second.errors, errName)) 
										  || err < bestErrors[errName].second.errors[errName]))
			{
				newBest = true;
				bestErrors[errName] = make_pair(epoch, currentErrors);
			}
		}
		return newBest;
	}
	void print_best_errors(const string& name, const map<string, pair<int, DatasetErrors> >& bestErrors) const
	{
		out << name << " set errors for best networks" << endl;
		LOOP(const PSPIDE& p, bestErrors)
		{
			prt_line(out);
			out << "epoch " << p.second.first << " (" << p.first << ")" << endl << p.second.second;
		}
	}
	DatasetErrors& calculate_errors(DataList& data, DatasetErrors& errors)
	{
		errors.clear();
		for (DataSequence* seq = data.start(); seq; seq = data.next_sequence())
		{
			if (verbose)
			{					
				out << "data sequence:" << endl;
				out << "file = " << data.dataset->filename << endl;
				out << "index = " << data.seqIndex << endl; 
				out << *seq;
			}	
			net->calculate_errors(*seq);
			errors.add_seq_errors(netErrors, netNormFactors);
			if (verbose)
			{
				net->print_output_shape(out);
				out << "errors:" << endl;
				out << netErrors << endl;
			}
		}
		errors.normalise();
		return errors;
	}
	void calculate_all_errors()
	{
		print_datasets();
		boost::timer t;
		if (trainFiles.numSequences)
		{
			out << "calculating train errors..." << endl;
			calculate_errors(trainFiles, trainErrors);
		}
		if (valFiles.numSequences)
		{
			out << "calculating validation errors..." << endl;
			calculate_errors(valFiles, valErrors);
		}
		if (testFiles.numSequences)
		{
			out << "calculating test errors..." << endl;
			calculate_errors(testFiles, testErrors);
		}
		real_t seconds = t.elapsed();
		if (trainFiles.numSequences)
		{
			out << "train errors:" << endl;
			out << trainErrors;
		}
		if (valFiles.numSequences)
		{
			prt_line(out);
			out << "validation errors:" << endl;  
			out << valErrors;
		}
		if (testFiles.numSequences)
		{
			prt_line(out);
			out << "test errors:" << endl;  
			out << testErrors;
		}
		int numSequences = trainFiles.numSequences + testFiles.numSequences + valFiles.numSequences;
		if (numSequences)
		{
			prt_line(out);
			out << numSequences << " sequences tested in ";
			print_time(seconds, out);
			out << endl;
			out << "average ";
			print_time(seconds/numSequences, out);
			out << " per sequence" << endl;
		}
		else
		{
			out << "WARNING: all data sets empty" << endl;
		}
	}
};

#endif
