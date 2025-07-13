% Threshold at 50%
cc_g_recall_50 = [ 0.75 0.79 0.93 1 1];
cc_g_precision_50 = [ 0.96 0.88 0.57 0.16 0.16];
plot(cc_g_recall_50, cc_g_precision_50, 'r', 'LineWidth', 3)
hold on;

% Threshold at 55%
cc_g_recall_55 = [ 0.73 0.82 0.91 1 1];
cc_g_precision_55 = [ 1 0.97 0.67 0.24 0.24];
plot(cc_g_recall_55, cc_g_precision_55, 'g', 'LineWidth', 3)
hold on;

% Threshold at 60%
cc_g_recall_60 = [ 0.67 0.78 0.91 1 1];
cc_g_precision_60 = [ 1 0.95 0.66 0.25 0.25];
plot(cc_g_recall_60, cc_g_precision_60, 'b', 'LineWidth', 3)
hold on;

% Threshold at 65%
cc_g_recall_65 = [ 0.59 0.74 0.93 1 1];
cc_g_precision_65 = [ 1 1 0.87 0.46 0.46];
plot(cc_g_recall_65, cc_g_precision_65, 'c', 'LineWidth', 3)
hold on;

% Threshold at 70%
cc_g_recall_70 = [ 0.58 0.72 0.81 0.90 0.94];
cc_g_precision_70 = [ 1 1 0.86 0.59 0.36];
plot(cc_g_recall_70, cc_g_precision_70,'m', 'LineWidth', 3)
hold on;

% Threshold at 75%
cc_g_recall_75 = [ 0.32 0.40 0.65 0.82 0.98];
cc_g_precision_75 = [ 1 1 0.94 0.73 0.38];
plot(cc_g_recall_75, cc_g_precision_75, 'Color',[150/255 200/255 70/255], 'LineWidth', 3)
hold on;

% Threshold at 80%
cc_g_recall_80 = [ 0.13 0.30 0.43 0.60 0.87];
cc_g_precision_80 = [ 1 1 0.94 0.84 0.48];
plot(cc_g_recall_80, cc_g_precision_80, 'k', 'LineWidth', 3)
hold on;


x = [0 1];
plot(x,x, 'k')

legend('50%','55%','60%','65%','70%','75%','80%',7); 

axis([0 1 0 1]);
set(gca,'XTick',[0 0.2 0.4 0.6 0.8 1])
set(gca,'YTick',[0 0.2 0.4 0.6 0.8 1])

xlabel('Recall');
ylabel('Precision');
