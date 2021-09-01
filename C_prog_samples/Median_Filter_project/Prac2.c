
#include "Prac2.h"
#include <iostream>
//------------------------------------------------------------------------------

// This is each thread's "main" function.  It receives a unique ID
void* Thread_Main(void* Parameter){
 int ID = *((int*)Parameter);

 pthread_mutex_lock(&Mutex);
  printf("Hello from thread %d\n", ID);
 pthread_mutex_unlock(&Mutex);

 for(int j = 0; j < 1e6; j++); // Do some heavy calculation

 pthread_mutex_lock(&Mutex);
  printf("Thread %d: I QUIT!\n", ID);
 pthread_mutex_unlock(&Mutex);

 return 0;
}
//------------------------------------------------------------------------------
/* Function to print an array */
void printArray(int pArr[], int pSize)
{
    int i;
    for (i=0; i < pSize; i++){
        printf("%d ", pArr[i]);
    }
    printf("\n----------------------------------------------------------------------------\n");
}

//------------------------------------------------------------------------------
/**
 * @brief: Convert the given RGB to Hex equivalent.
 * @return  Int equivalent of the converted Hex number
 */
int getRGB_Integer(unsigned char pR, unsigned char pG, unsigned char pB){
 return (((int)pR & 0xff) << 16) + (((int)pG & 0xff) << 8) + ((int)pB & 0xff);
}
//-----------------------------------------------------------------------------
void swap(int* pX, int* pY){
 int temp = *pX;
 *pX = *pY;
 *pY = temp;
}
//-----------------------------------------------------------------------------
int partition(int* pArray, int pLower_i, int pHigher_i){
 int pivot = pArray[pHigher_i];
 int i = pLower_i - 1;

 for(int j = pLower_i; j < pHigher_i; j++){
   if(pArray[j] < pivot){
     i++;
     swap(&pArray[i], &pArray[j]);
   }
 }
 
 swap(&pArray[i + 1],&pArray[pHigher_i]);
 return (i + 1);
}
//-----------------------------------------------------------------------------
void quickSort(int* pArray, int pLower_i, int pHigher_i){
 if(pLower_i < pHigher_i){
   int partition_index = partition(pArray, pLower_i, pHigher_i);
   
   // Recursively sort items before and after partition index
   quickSort(pArray, pLower_i, partition_index - 1);
   quickSort(pArray, partition_index + 1, pHigher_i);
 }
}
//-----------------------------------------------------------------------------
int main(int argc, char** argv){
  printf("Code is running...");
  Input.Read("Data/small.jpg");
  int winSizeX = 9;
  int winSizeY = 9;
  
  if(!Output.Allocate(Input.Width, Input.Height, Input.Components)){ 
    return -2;
  }   
  int* neighborPixels;  

  tic();
  int x, y, fx, fy, winEdgeX, winEdgeY;
  for(y = 0; y < Input.Height; y++){
    winEdgeY = y - (winSizeY/2);
    int diffY = 0;
    // Handle the edge cases in Y direction
    if(winEdgeY < 0){
      diffY = -winEdgeY;
      winEdgeY = 0;
    }
    if((winEdgeY + winSizeY) > Input.Height){
      diffY = (winEdgeY + winSizeY) - Input.Height;
    }

    for(x = 0; x < Input.Width*Input.Components; x+= Input.Components){
      // get all the neighboring pixels and put them in and array of 81 items
      // Window will be shrinked near the boundries
      winEdgeX = x - (((int)(winSizeX/2))*Input.Components);

      // Handle the edge cases in X direction
      int diffX = 0;
      if(winEdgeX < 0){
        diffX = -winEdgeX;
        winEdgeX = 0;
      }
      if((winEdgeX + (winSizeX*Input.Components)) > (Input.Width*Input.Components)){
        diffX = (winEdgeX + (winSizeX*Input.Components)) - (Input.Width*Input.Components);
      }
 
      int n = (winSizeX - (diffX/Input.Components))*(winSizeY - diffY);
      neighborPixels = new int[n];
      int i = 0;
      for(fy = 0; fy < (winSizeY - diffY); fy++){
	for(fx = 0;fx < ((winSizeX*Input.Components) - diffX); fx+=Input.Components){
	  neighborPixels[i++] = getRGB_Integer(Input.Rows[winEdgeY+fy][winEdgeX+fx+0],Input.Rows[winEdgeY+fy][winEdgeX+fx+1],Input.Rows[winEdgeY+fy][winEdgeX+fx+2]);
        }
      }
      // Sort this list
      quickSort(neighborPixels, 0, n - 1);
      // Save the median of the sorted list to the current pixel
      int median = neighborPixels[(n - 1)/2];
      delete[] neighborPixels;

      // Convert the median back to RGB (3 components)
      Output.Rows[y][x + 0] = (unsigned char)((median >> 16) & 0xff);
      Output.Rows[y][x + 1] = (unsigned char)((median >> 8) & 0xff);
      Output.Rows[y][x + 2] = (unsigned char)(median & 0xff);
    }    
  }
  printf("Run time is: %lg ms \n", toc()/(1e-3));

  // Write the output image
  if(!Output.Write("Data/Output.jpg")){
    printf("Cannot write image\n");
    return -3;
  }

  printf("Finish...");

  return 0;
}
//------------------------------------------------------------------------------
