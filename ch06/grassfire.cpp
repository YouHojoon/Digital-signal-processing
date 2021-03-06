#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

int push(short* stackx, short* stacky, int arr_size, short vx, short vy, int* top)
{
	if (*top >= arr_size) return(-1);
	(*top)++;
	stackx[*top] = vx;
	stacky[*top] = vy;
	return(1);
}

int pop(short* stackx, short* stacky, short* vx, short* vy, int* top)
{
	if (*top == 0) return(-1);
	*vx = stackx[*top];
	*vy = stacky[*top];
	(*top)--;
	return(1);
}


// GlassFire 알고리즘을 이용한 라벨링 함수
void m_BlobColoring(BYTE* CutImage, int height, int width)
{
	int i, j, m, n, top, area, Out_Area, index, BlobArea[1000];
	long k;
	short curColor = 0, r, c;
	//	BYTE** CutImage2;
	Out_Area = 1;


	// 스택으로 사용할 메모리 할당
	short* stackx = new short[height * width];
	short* stacky = new short[height * width];
	short* coloring = new short[height * width];

	int arr_size = height * width;

	// 라벨링된 픽셀을 저장하기 위해 메모리 할당

	for (k = 0; k < height * width; k++) coloring[k] = 0;  // 메모리 초기화

	for (i = 0; i < height; i++)
	{
		index = i * width;
		for (j = 0; j < width; j++)
		{
			// 이미 방문한 점이거나 픽셀값이 255가 아니라면 처리 안함
			if (coloring[index + j] != 0 || CutImage[index + j] != 255) continue;
			r = i; c = j; top = 0; area = 1;
			curColor++;

			while (1)
			{
			GRASSFIRE:
				for (m = r - 1; m <= r + 1; m++)
				{
					index = m * width;
					for (n = c - 1; n <= c + 1; n++)
					{
						//관심 픽셀이 영상경계를 벗어나면 처리 안함
						if (m < 0 || m >= height || n < 0 || n >= width) continue;

						if ((int)CutImage[index + n] == 255 && coloring[index + n] == 0)
						{
							coloring[index + n] = curColor; // 현재 라벨로 마크
							if (push(stackx, stacky, arr_size, (short)m, (short)n, &top) == -1) continue;
							r = m; c = n; area++;
							goto GRASSFIRE;
						}
					}
				}
				if (pop(stackx, stacky, &r, &c, &top) == -1) break;
			}
			if (curColor < 1000) BlobArea[curColor] = area;
		}
	}

	float grayGap = 255.0f / (float)curColor;

	// 가장 면적이 넓은 영역을 찾아내기 위함 
	for (i = 1; i <= curColor; i++)
	{
		if (BlobArea[i] >= BlobArea[Out_Area]) Out_Area = i;
	}
	// CutImage 배열 클리어~
	for (k = 0; k < width * height; k++) CutImage[k] = 255;

	// coloring에 저장된 라벨링 결과중 (Out_Area에 저장된) 영역이 가장 큰 것만 CutImage에 저장
	for (k = 0; k < width * height; k++)
	{
		if (coloring[k] == Out_Area) CutImage[k] = 0;  // 가장 큰 것만 저장
		//CutImage[k] = (unsigned char)(coloring[k] * grayGap);
	}

	delete[] coloring;
	delete[] stackx;
	delete[] stacky;
}
// 라벨링 후 가장 넓은 영역에 대해서만 뽑아내는 코드 포함
void thresholding(BYTE* image, BYTE* output, int imgSize, int th) {
	for (int i = 0; i < imgSize; i++)
		(image[i] > th) ? output[i] = 0 : output[i] = 255;
}
int cntPixel(BYTE *image, int Cx, int Cy,int W) {
	int cnt=0;
	
	for (int i = Cy-1; i <= Cy+1; i++) {
		for (int j = Cx-1; j <= Cx+1; j++) {
			if (image[i * W + j] == 255)
				cnt++;
		}
	}

	return --cnt;
}

void edgeDetection(BYTE *image, BYTE* output, int W, int H) {

	BYTE* tmp = (BYTE*)malloc(H * W * sizeof(BYTE));
	thresholding(image, tmp, W * H, 50);

	for (int i = 0; i < W * H; i++)
		output[i] = image[i];

	for (int i = 1; i < H-1; i++) {
		for (int j = 1; j < W-1; j++) {
			if (tmp[i*W+j]==255 && cntPixel(tmp, j, i, W) != 8) 
				output[i * W + j] = 255;
		}
	}
	
	free(tmp);
}
void drawBox(BYTE* biImage, BYTE* output, int W, int H) {
	int x1, x2, y1, y2;

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (biImage[i * W + j] == 0) {
				y2 = i;
				break;
			}
		}
	}

	for (int i = H - 1; i >= 0; i--) {
		for (int j = W - 1; j >= 0; j--) {
			if (biImage[i * W + j] == 0) {
				y1 = i;
				break;
			}
		}
	}

	for (int i = 0; i < W; i++) {
		for (int j = 0; j < H; j++) {
			if (biImage[j * W + i] == 0) {
				x2 = i;
				break;
			}
		}
	}

	for (int i = W - 1; i >= 0; i--) {
		for (int j = H - 1; j >= 0; j--) {
			if (biImage[j * W + i] == 0) {
				x1 = i;
				break;
			}
		}
	}

	for (int i = x1; i <= x2; i++)
		output[y1 * W + i] = 255;

	for (int i = x1; i <= x2; i++)
		output[y2 * W + i] = 255;

	for (int i = y1; i <= y2; i++)
		output[i * W + x1] = 255;

	for (int i = y1; i <= y2; i++)
		output[i * W + x2] = 255;
}

void drawCross(BYTE* biImage, BYTE* output, int W, int H) {
	int sumX = 0, sumY = 0, cnt = 0;

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			if (biImage[i * W + j] == 0) {
				sumX += j;
				sumY += i;
				cnt++;
			}
		}
	}

	sumX /= cnt;
	sumY /= cnt;

	for (int i = 0; i < H; i++) 
		output[i * W + sumX] = 255;
	
	for (int i = 0; i < W; i++) 
		output[sumY * W + i] = 255;
}
int main(void) {
	BITMAPFILEHEADER hf;
	BITMAPINFOHEADER hinfo;
	RGBQUAD hRGB[256];
	FILE* fp;
	int imgSize;

	fp = fopen("pupil1.bmp", "rb");
	if (fp == NULL) {
		perror("fopen");
		exit(1);
	}

	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hinfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(hRGB, sizeof(RGBQUAD), 256, fp);
	
	int W = hinfo.biWidth;
	int H = hinfo.biHeight;
	imgSize = W * H;

	BYTE* image = (BYTE*)malloc(imgSize);
	BYTE* output = (BYTE*)malloc(imgSize);
	BYTE* label = (BYTE*)malloc(imgSize);

	fread(image, sizeof(BYTE), imgSize, fp);
	fclose(fp);
	
	
	edgeDetection(image, output, W, H);

	fp = fopen("label.bmp", "wb");
	fwrite(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&hinfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(output, sizeof(BYTE), imgSize, fp);
	fclose(fp);

	thresholding(image, output, imgSize, 50);
	m_BlobColoring(output ,hinfo.biHeight, hinfo.biWidth);

	drawBox(output, image, W, H);
	drawCross(output, image, W, H);
	
	fp = fopen("outbox.bmp", "wb");
	fwrite(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&hinfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(image, sizeof(BYTE), imgSize, fp);
	fclose(fp);

	free(image);
	free(output);

	return 0;

}

