


#include <iostream>
#include <fstream>

using namespace std;

typedef struct
{
	float x, y, z;
} VertexType;

typedef struct
{
	int vIndex1, vIndex2, vIndex3;
	int tIndex1, tIndex2, tIndex3;
	int nIndex1, nIndex2, nIndex3;
} FaceType;

void GetModelFilename(char* filename);
bool ReadFileCounts(char* filename, int& vertexCount, int& textureCount, int& normalCount, int& faceCount);
bool LoadDataStructures(char* filename, int vertexCount, int textureCount, int normalCount, int faceCount);

int main()
{
	bool result;
	char filename[256];

	int vertexCount, textureCount, normalCount, faceCount;

	char garbage;

	GetModelFilename(filename);

	result = ReadFileCounts(filename, vertexCount, textureCount, normalCount, faceCount);
	if (!result)
	{
		return -1;
	}

	cout << endl;
	cout << "���ؽ� : " << vertexCount << endl;
	cout << "UVs : " << textureCount << endl;
	cout << "��� : " << normalCount << endl;
	cout << "���̽� : " << faceCount << endl;

	result = LoadDataStructures(filename, vertexCount, textureCount, normalCount, faceCount);
	if (!result)
	{
		return -1;
	}

	cout << "\nFile has been converted." << endl;
	cout << "\nDo you wish to exit (y/n)?";
	cin >> garbage;
	
	return 0;
}

void GetModelFilename(char* filename)
{
	bool done;
	ifstream fin;

	done = false;
	while(!done)
	{
		cout << "�� ���� �̸��� �Է��ϼ���. :: ";
		cin >> filename;

		fin.open(filename);

		if (fin.good())
		{
			done = true;
		}
		else
		{
			fin.clear();

			cout << endl;
			cout << "���� " << filename << "�� �� �� �����ϴ�." << endl << endl;
		}
	}
}

bool ReadFileCounts(char* filename, int& vertexCount, int& textureCount, int& normalCount, int& faceCount)
{
	ifstream fin;
	char input;

	vertexCount = 0;
	textureCount = 0;
	normalCount = 0;
	faceCount = 0;

	fin.open(filename);

	if (fin.fail())
	{
		return false;
	}

	fin.get(input);
	while(!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);
			if (input == ' ') { vertexCount++; }
			else if (input == 't') { textureCount++; }
			else if (input == 'n') { normalCount++; }
		}

		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ') { faceCount++; }
		}

		while (input != '\n')
		{
			fin.get(input);
		}

		fin.get(input);
	}

	fin.close();
	
	return true;
}

bool LoadDataStructures(char* filename, int vertexCount, int textureCount, int normalCount, int faceCount)
{
	VertexType* vertices, * texcoords, * normals;
	FaceType* faces;

	ifstream fin;

	int vertexIndex, texcoordIndex, normalIndex, faceIndex;
	int vIndex, tIndex, nIndex;
	
	char input, input2;

	ofstream fout;

	vertices = new VertexType[vertexCount];
	if (!vertices)
	{
		return false;
	}

	texcoords = new VertexType[textureCount];
	if (!texcoords)
	{
		return false;
	}

	normals = new VertexType[normalCount];
	if (!normals)
	{
		return false;
	}

	faces = new FaceType[faceCount];
	if (!faces)
	{
		return false;
	}

	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;

	fin.open(filename);

	if (fin.fail())
	{
		return false;
	}

	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);

			
			if (input == ' ')
			{
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

				
				vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
				vertexIndex++;
			}

			
			if (input == 't')
			{
				fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;

				
				texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
				texcoordIndex++;
			}

			
			if (input == 'n')
			{
				fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;

				
				normals[normalIndex].z = normals[normalIndex].z * -1.0f;
				normalIndex++;
			}
		}

		
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				
				fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> input2 >> faces[faceIndex].nIndex3
					>> faces[faceIndex].vIndex2 >> input2 >> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2
					>> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >> faces[faceIndex].nIndex1;
				faceIndex++;
			}
		}

		
		while (input != '\n')
		{
			fin.get(input);
		}

		
		fin.get(input);
	}

	
	fin.close();

	
	fout.open("model.txt");

	
	fout << "Vertex Count: " << (faceCount * 3) << endl;
	fout << endl;
	fout << "Data:" << endl;
	fout << endl;

	
	for (int i = 0; i < faceIndex; i++)
	{
		vIndex = faces[i].vIndex1 - 1;
		tIndex = faces[i].tIndex1 - 1;
		nIndex = faces[i].nIndex1 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

		vIndex = faces[i].vIndex2 - 1;
		tIndex = faces[i].tIndex2 - 1;
		nIndex = faces[i].nIndex2 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

		vIndex = faces[i].vIndex3 - 1;
		tIndex = faces[i].tIndex3 - 1;
		nIndex = faces[i].nIndex3 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;
	}

	
	fout.close();

	
	if (vertices)
	{
		delete[] vertices;
		vertices = nullptr;
	}
	if (texcoords)
	{
		delete[] texcoords;
		texcoords = nullptr;
	}
	if (normals)
	{
		delete[] normals;
		normals = nullptr;
	}
	if (faces)
	{
		delete[] faces;
		faces = nullptr;
	}

	return true;
}
