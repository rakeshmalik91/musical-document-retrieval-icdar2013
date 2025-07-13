echo off
::rmdir /Q /S DATA\TEXT_SR
::rmdir /Q /S DATA\TEXTLINE
rmdir /Q /S DATA\OUTPUT
cls

echo Removing staff lines...
::mkdir DATA\TEXT_SR
::remove_staffline DATA\TEXT DATA\TEXT_SR
echo.

echo Extracting text lines...
::extract_line DATA\TEXT_SR DATA\TEXTLINE
echo.

echo Retrieving (Zernike & KMeans)...
"search (Zernike-KMeans)" 0 99
"search (Zernike-KMeans)" 100 199
"search (Zernike-KMeans)" 200 299
"search (Zernike-KMeans)" 300
echo.

echo Retrieving (Zernike & SOM)...
"search (Zernike-SOM)" 0 99
"search (Zernike-SOM)" 100 199
"search (Zernike-SOM)" 200 299
"search (Zernike-SOM)" 300
echo.

echo Retrieving (400 dimensional feature & KMeans)...
"search (400dim-KMeans)" 0 99
"search (400dim-KMeans)" 100 199
"search (400dim-KMeans)" 200 299
"search (400dim-KMeans)" 300
echo.

echo Retrieving (400 dimensional feature & SOM)...
"search (400dim-SOM)" 0 99
"search (400dim-SOM)" 100 199
"search (400dim-SOM)" 200 299
"search (400dim-SOM)" 300
echo.

echo Retrieving (DTW)...
"search (DTW)" 0 99
"search (DTW)" 100 199
"search (DTW)" 200 299
"search (DTW)" 300
echo.

echo Creating graph...
graph
echo.

pause