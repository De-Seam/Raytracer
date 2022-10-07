#include "precomp.h"
#include "file.h"

char* ReadFile(const char* filename) //Copied from SDL wiki (libsdl.org)
{
	SDL_RWops* rw = SDL_RWFromFile(filename, "rb");
	if (rw == NULL) return NULL;

	size_t res_size = static_cast<size_t>(SDL_RWsize(rw));
	char* res = (char*)malloc(res_size + 1);

	size_t nb_read_total = 0, nb_read = 1;
	char* buf = res;
	while (nb_read_total < res_size && nb_read != 0) {
		nb_read = SDL_RWread(rw, buf, 1, (res_size - nb_read_total));
		nb_read_total += nb_read;
		buf += nb_read;
	}
	SDL_RWclose(rw);
	if (nb_read_total != res_size) {
		free(res);
		return NULL;
	}

	res[nb_read_total] = '\0';
	return res;
}

om::vec3 GetNumbersFromLine(Vector<char> line)
{
	om::vec3 returnNumber;
	int numberStart;
	float number;
	int decimalLoc;
	int numberLength;
	int decimalLength;
	bool negative = false;
	int a = 1;
	for (int i = 0; i < 3; i++)
	{
		numberStart = a;
		number = 0;
		decimalLoc = 0;
		while (numberStart < line.size() && line[numberStart] == ' ')
		{
			numberStart++;
		}
		if (line[numberStart] == '-')
		{
			negative = true;
			numberStart++;
		}
		a = numberStart;
		while (a < line.size() && line[a] != ' ')
		{
			if (line[a] == '.')
				decimalLoc = a;
			a++;
		}
		if (decimalLoc == 0)
			decimalLoc = a;
		numberLength = decimalLoc - numberStart;
		decimalLength = a - numberLength - numberStart;

		int temp = a - decimalLength;
		int modifier = 0;
		for (int j = numberLength; j > 0 - decimalLength; j--)
		{
			if (temp - j == decimalLoc)
			{
				modifier = 1;
				continue;
			}
			number += (float)(((line[temp - j] - '0') * pow(10, j - 1 + modifier)));
		}
		if (negative)
			number = number * -1;
		//fscanf(fileContent, "");
		switch (i)
		{
		case 0:
			returnNumber.x = number;
			break;
		case 1:
			returnNumber.y = number;
			break;
		default:
			returnNumber.z = number;
			break;
		}
	}
	return returnNumber;
}

Vector<Vector<char>> CAtoCV(char* charArray, Vector<Vector<char>>& line)
{
	size_t charCount = SDL_strlen(charArray);
	line.reserve((int)charCount);
	Vector<char> currentLine;
	for (size_t i = 0; i < charCount; i++)
	{
		if (charArray[i] == '\r')
			i++;

		if (charArray[i] == '\n')
		{
			line.push_back(currentLine);
			currentLine.resize(0);
		}
		else
			currentLine.push_back(charArray[i]);
	}
	if(currentLine.size() > 0)
		line.push_back(currentLine);
	return line;
}

int GetCharAmount(Vector<Vector<char>> line, char c)
{
	int returnVal = 0;
	for (int i = 0; i < line.size(); i++)
	{
		if (line[i][0] == c)
			returnVal++;
	}
	return returnVal;
}



void LoadObj(const char* filename, Vector<Triangle>* triangle, FColor color, float scale, int material)
{
	printf("loading: ");
	printf(filename);
	printf("\n");
	Vector<Vector<char>> line;
	CAtoCV(ReadFile(filename), line);
	Vector<om::vec3> vertice(GetCharAmount(line, 'v'));
	triangle->reserve(GetCharAmount(line, 'f'));
	om::vec3 numbers;
	for (int i = 0; i < line.size(); i++)
	{
		switch (line[i][0])
		{
		case 'v':
		{
			om::vec3 v = GetNumbersFromLine(line[i]);
			v.x = -v.x; //flip x axis
			vertice.push_back(v);
			vertice.back() *= scale;
			break;
		}
		case 'f':
		{
			numbers = GetNumbersFromLine(line[i]) - om::vec3(1, 1, 1);
			triangle->push_back(Triangle(vertice[(int)numbers.x], vertice[(int)numbers.y], vertice[(int)numbers.z], material, color));
			break;
		}
		default:
			break;
		}
	}
}