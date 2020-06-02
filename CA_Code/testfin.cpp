#include<iostream>
#include<graphics.h>
#include<process.h>
#include<stdlib.h>
#include<stdio.h>
#include<conio.h>
#include<dos.h>
#include<string.h>
#include<ctype.h>
#include<windows.h>
#include<fstream>
#include<bits/stdc++.h>


using namespace std;



int gdriver = DETECT, gmode, errorcode;
// KNOW YOUR COMPOUND
int goback_iupac=0;
int i,j,k,l,m,n;//i,j,k, used in loop and l for carbon no.
string iupac;// input name
char root[11][5]={" ","Meth","Eth","Prop","But","Pent","Hex","Hept","Oct","Non","Dec"};//Word Root
int noc=0;// no of carbons
//char op;//option for main menu
int loc[100];// to store bond position
int tot=0; // to count no. of numbers entered
int len=0,bond[100],ilen=0; //len is length of word root and bond[] is bond number ilen is length of iupac
int check=0,multi=0; // for conditions when  location is not entered eg:Pentane
//--- Bond var -----------
int bx=0,tlen=0;// x to check no. of '-' and tlen to determine length of temp var
char temp[100];// to store locant for multiple bonds
int val=0,a=0;
char mtemp[2][100];
//----- Alkyl var --------
char alkyl[5][5]={" ","meth","eth","prop","but"};
char altemp[100];// to store secondary prefix
int alloc[100]; // to store alkyl group position
int alknoc[100]; // to store no. of carbons on alkyl group
int cbroot=0; // charecters before word root
int multibond=0;
int dashpos[100];// to store locationof dashes before word root
int num=1,cpy=0; // num to cal no. of digit before word root and copy to copy values of alknoc var
//----- Structure var -------
int pos[100],an[100];
int bo[100],bp[100];
char hal[100];
int hallen=0;
int halp[100];
//------ Halogen -------------------
char halogen[][10]={"Fluoro","Chloro","Bromo","Iodo"};
char haltemp[100]; // to store halogens
int halolen=-1; // length of haltemp
int hhy=0; // no. of hypen for halogen
int hdashpos[100]; // location of dashes
char haltype[100]; // to store halogoen eg F-fluorine
int htot=0;  // number of loacants
int halloc[100]; // loaction of halo
int hnum=0;   // to cal no. of digits before alkyl group
char hcopy='q';   // to copy val of haltemp var
//------ Functional Group var -------------
int cabond=0;     // charecters after bond ie after ene or yne
char functemp[100]; // to store functional group
char prefix[][20]={" ","Cyano","Hydroxy","Amino","Keto","Carboxy","Oxo","Carbamoyl"};
int funclen=0;// len of functemp
int fdash;
int ftot=0;
char funccmp[100];
int floc[100];
int ftype[100];
int fval;
char fgroup[10][25]={"ether","sulphonic acid","nitrile","ol","amine","one","oic acid","al","amide","oate"};
int prelen=0;
int pretot=0;
int preloc[100];
int prehy=0;
int predashpos[100];
//-----  IDK ----------
char pretemp[100];
int cbhalo=0;
int cbhalogen[100];

void formula();
void EncodeString(string, fstream*);
void DecodeString(fstream*);


void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}


//---------------Creating min heap for Huffman tree------------

map<char, string> codes;

// ---------Hauffman Tree Node------------
struct MinHeapNode{
  char data;
  unsigned freq;
  MinHeapNode *left, *right;
  MinHeapNode(char data, unsigned freq){
    left = right = NULL;
    this->data = data;
    this->freq = freq;
  }
};

struct compare{
	bool operator()(MinHeapNode *l, MinHeapNode *r){
		return (l->freq > r->freq);
	}
};

// ---------------To recreate the Huffman Tree---------------
struct Node{
  char data;
  Node *left, *right;
  Node(char data, Node* l, Node* r){
    left = l;
    right = r;
    this->data = data;
  }
};

//----------To write a generated code into a file-----------
void storeCodes(struct MinHeapNode* root, string str){
  if(!root)
    return;
  if(root->data != '$')  //Arbitrary character
    codes[root->data] = str;

  storeCodes(root->left, str + "0");
  storeCodes(root->right, str + "1");
}

// ----------------To write the Min Heap data into the file-----------
void EncodeNode(struct MinHeapNode* root, fstream* fio){
  if(root->left == NULL and root->right == NULL){
    *fio<<1;
    *fio<<root->data;
  }
  else{
    *fio<<0;
    EncodeNode(root->left, fio);
    EncodeNode(root->right, fio);
  }
}


priority_queue<MinHeapNode*, vector<MinHeapNode*>, compare> minHeap;
// Function to build the hauffman Tree
void HuffmanCodes(string data, int size){
  minHeap = priority_queue<MinHeapNode*, vector<MinHeapNode*>, compare>();
  map<char, int> freq; //To store the frequecy of each character
  for(int i=0; i<size; ++i)
    freq[data[i]]++;
  struct MinHeapNode *left, *right, *top;

  // Creating a priority queue for min heap
  for(int i=0; i<size; ++i)
    minHeap.push(new MinHeapNode(data[i], freq[data[i]]));

  while(minHeap.size() != 1){
    left = minHeap.top();
    minHeap.pop();

    right = minHeap.top();
    minHeap.pop();

    // Creating a new internal node with freq = sum of freq of left and right
    top = new MinHeapNode('$', left->freq + right->freq);
    top->left = left;
    top->right = right;

    minHeap.push(top);

  }
  storeCodes(minHeap.top(), "");
}

struct Node* ReadNode(fstream *fio){
  char c;
  *fio>>c;
  if(c == '1'){
    *fio>>c;
    return new Node(c, NULL, NULL);
  }
  else{
    Node *leftChild = ReadNode(fio);
    Node *rightChild = ReadNode(fio);
    return new Node('$', leftChild, rightChild);
  }
}

string decodeFile(fstream *fio){
  string ans = "";
  string s;
  struct Node* root = ReadNode(fio);
  struct Node* curr = root;
  *fio>>s;
  for(int i=0; i<s.length(); i++){
    if(s[i] == '0')
      curr = curr->left;
    else
      curr = curr->right;

    if(curr->left == NULL and curr->right == NULL){
      ans += curr->data;
      curr = root;
    }
  }
  return ans;
}

void EncodeString(string str, fstream* fio){
	int size = str.length();
	HuffmanCodes(str, size);
	EncodeNode(minHeap.top(), fio);
	*fio<<" ";
	string encodedString = "";
	for(int i=0; i<size; ++i)
    	encodedString += codes[str[i]];
    *fio<<encodedString<<endl;
}

void DecodeString(){
	fstream fio;
	fio.open("compounds.txt", ios::in | ios::out);
	fio.seekg(0, ios::beg);
	string decodedString;
	vector<string> history;
	int i = 1, choice;
	while(fio){
		decodedString = decodeFile(&fio);
		history.push_back(decodedString);
		cout<<i++<<". "<<decodedString<<endl;
	}
	fio.close();
	cin>>choice;
	iupac = history[choice-1];
	formula();
}


//--------- FUNCTIONS FOR IUPAC ----------------
void multiple()       //
{
	multi=1;
	k=-1;
	j=-1;
	val=0;
	for(k=0;iupac[k]!=root[noc][0];k++)
	{
		cbroot++;
	}
	k=-1;
	for(int i=len+cbroot;i<ilen;i++)
	{
		if(iupac[i]=='-')
		val++;
		if(val==1)
		{
			i++;
			j++;
			mtemp[0][j]=iupac[i];
		}
		if(val==2)
		{
			if(iupac[i-2]=='e')
			{
				bond[0]=2;
			}
			else if(iupac[i-2]=='y')
			{
				bond[0]=3;
			}
		}
		if(val==3)
		{
			k++;
			i++;
			mtemp[1][k]=iupac[i];
		}
		if(val==4)
		{
			if(iupac[i-2]=='e')
			{
				bond[1]=2;
			}
			else if(iupac[i-2]=='y')
			{
				bond[1]=3;
			}
		}
	}


// --------- Multi Location ----------------

	for(int i=0;i<=j;i++)
	{
		bp[i+1]=mtemp[0][i]-48;
		bo[i+1]=bond[0];
	}
	for(int i=0;i<=k;i++)
	{
		bp[i+2+j]=mtemp[1][i]-48;
		bo[i+2+j]=bond[1];
	}
	tot=j+k+2;
}
void map_iupac()
{
	gotoxy(1,1);
	for( int mapy=3;mapy<=25;mapy++)
	{
		gotoxy(1,mapy);
		cout<<"#";
		gotoxy(80,mapy);
		cout<<"#";
	}
	for(int mapx=2;mapx<=78;mapx+=2)
	{
		gotoxy(mapx+1,2);
		cout<<"#";
		gotoxy(mapx,24);
		cout<<"#";
	}
}
void reset_iupac()
{
	/*for(i=0;i<250;i++)
	{
		iupac[i]='.';
	}*/
	for(int i=0;i<100;i++)
	{
		loc[i]=-1;
		bond[i]=-1;
		alloc[i]=-1;
		alknoc[i]=-1;
		dashpos[i]=-1;
		pos[i]=-1;
		an[i]=-1;
		bo[i]=-1;
		bp[i]=-1;
		halp[i]=-1;
		hdashpos[i]=-1;
		halloc[i]=-2;
		floc[i]=-2;
		ftype[i]=-2;
		preloc[i]=-1;
		predashpos[i]=-1;
		pretemp[i]=-1;
		temp[i]=0;
		mtemp[0][i]=0;
		mtemp[1][i]=0;
		altemp[i]=0;
		haltemp[i]=0;
		functemp[i]=0;
		funccmp[i]=0;


	}
	noc=0;
	tot=0;
	len=0;
	ilen=0;
	check=0;
	multi=0;
	bx=0;
	tlen=0;
	val=0;
	a=0;
	cbroot=0;
	multibond=0;
	num=1;
	cpy=0;
	hallen=0;
	halolen=-1;
	hhy=0;
	htot=0;
	hnum=0;
	hcopy='q';
	cabond=0;
	funclen=0;
	fdash;
	ftot=0;
	fval=0;
	prelen=0;
	pretot=0;
	prehy=0;
	cbhalo=0;

}
void help_iupac()
{
	system("cls");
	initgraph(&gdriver, &gmode, "");
	setbkcolor(0);
	closegraph();
	gotoxy(25,4);
	cout<<"How to use this Program";
	cout<<"\n\n\tWelcome to 'Know Your Compound'!!! This program can be used to"<<endl;
	cout<<"\tdetermine the schematic structure of an organic compound. Due to"<<endl;
	cout<<"\tlimitations in size, only straight chain compounds with a max of"<<endl;
	cout<<"\t10 carbons in the main chain are supported."<<endl;
	cout<<"\t\tRULES TO USE THE COMPOUND ANALYSER:"<<endl;
	cout<<"\t\t@.The first letter of the wordroot should be in uppercase."<<endl;
	cout<<"\t\t@.The first letters of alkyl groups should be in lowercase."<<endl;
	cout<<"\t\t@.The first letters of prefix functional groups and "<<endl;
	cout<<"\t\t  halogen should be in uppercase."<<endl;
	cout<<"\t\t@.Functional group prefix should only be used if a"<<endl;
	cout<<"\t\t  functional group suffix has been used first."<<endl;
	cout<<"\t\t@.The positions of alkyl groups,halogens and functional"<<endl;
	cout<<"\t\t  groups should be specified with '-' and ',' as per"<<endl;
	cout<<"\t\t  IUPAC rules"<<endl;
	cout<<"\t\t@ Press f to see the list of functional groups supported"<<endl;
	cout<<"\n\t\tEg:8-Keto-5,6-Chloro-4,5-ethylDec-2-ene-1-oic acid";
	cout<<"\n\n\tPress 'b' to go back";
	map_iupac();
}
void formula()
{
	system("cls");
//	initgraph(&gdriver, &gmode, "");
	cleardevice();
	setbkcolor(8);
	reset_iupac();
	gotoxy(28,4);
	cout<<"Formula Analyser";
	cout<<"\n\n\tEnter IUPAC Name:";
	map_iupac();
	gotoxy(27,5);
	cin>>iupac;
	fstream fio;
	fio.open("compounds.txt", ios::in | ios::out | ios::trunc);
	EncodeString(iupac, &fio);
	fio.close();
	ilen=iupac.length();

// ----------- Detecting Character Before halogens ---------------------

	l=-1;
	for(int i=ilen;i>=0;i--)
	{
		for(j=7;j>=1;j--)
		{
			m=0;
			for(k=0;k<strlen(prefix[j]);k++)
			{
				if(iupac[i-k]==prefix[j][strlen(prefix[j])-1-k])
				m++;
			}
			if(m==strlen(prefix[j]))
			{
				l++;
				cbhalogen[l]=i;
			}
		}
	}
	cbhalo=cbhalogen[0];
	for(int i=0;i<=cbhalo;i++)
	{
		pretemp[i]=iupac[i];
	}
	int newlen=i;
//------------ Detecting Characters before alkyl group -------------
	for(i=ilen;i>=0;i--)
	{
		if(iupac[i]=='o'&&(iupac[i-1]=='r'||iupac[i-1]=='m'||iupac[i-1]=='d')&&iupac[i-2]=='o')
		break;
	}
	for(j=cbhalo,k=0;j<=i;j++,k++)
	{
		haltemp[k]=iupac[j];
	}
	halolen=strlen(haltemp);
//------------ Detecting Characters after bond ---------------------
	for(i=ilen-1;i>=0;i--)
	{
		if(iupac[i]=='e'&&iupac[i-1]=='n'&&(iupac[i-2]=='a'||iupac[i-2]=='e'||iupac[i-2]=='y'))
		break;
	}
	cabond=ilen-i-1;
//-----Case 1:  Word Root  ------------------------------------
	for(i=1;i<11;i++)
	{
		for(k=0;k<iupac.length();k++)
		{
			if(iupac[k]==root[i][0]&&iupac[k+1]==root[i][1]&&iupac[k+2]==root[i][2])
			{
				noc=i;//no of carbons in parent chain
				break;
			}
		}
	}


// ------------------Bond Type ------------------------------
	k=0;
	m=0;
	for(k=0;iupac[k]!=root[noc][0];k++)
	{
		m++;
	}
	len=strlen(root[noc]);
	ilen=iupac.length()-cabond;
	if(iupac[ilen-2]=='n'&&iupac[ilen-1]=='e')
	{
		for(i=len+m;i<ilen;i++)
		{
			if(iupac[i]=='-')
			{
				val++;
			}
		}
		if(val<=2)
		{

			if(iupac[ilen-3]=='a')
			{
				bond[0]=1;
				if(root[noc][len-1]==iupac[ilen-4])
				{
					check=1;
					bp[1]=1;
					bo[1]=0;
				}
			}
			else if(iupac[ilen-3]=='e')
			{
				bond[0]=2;
				if(root[noc][len-1]==iupac[ilen-4])
				{
					check=1;
					bp[1]=1;
					bo[1]=2;
				}
			}
			else if(iupac[ilen-3]=='y')
			{
				bond[0]=3;
				if(root[noc][len-1]==iupac[ilen-4])
				{
					check=1;
					bp[1]=1;
					bo[1]=3;
				}
			}
			else
			bond[0]=0;
		}
		if(val>=3)
		{
			 multiple();
		}
	}
//-------------- Bond position -------------------
	m=0;
	for(k=0;iupac[k]!=root[noc][0];k++)
	{
		m++;
	}
	for(i=len+m,j=0;i<ilen;i++,j++)
	{
		if(iupac[i]=='-')
		{
			bx++;
			if(bx==1)
			i++;
		}
		if(bx!=2)
		{
			temp[j]=iupac[i];
			tlen++;
		}
	}
	j=-1;

// --------------- Location of Bond ---------------------
	tot=0;
	if(multi==0)
	{
		for(i=0;i<tlen;i++)
		{
			if(temp[i]!=',')
			{
				j++;
				loc[j]=temp[i]-48;   //location of bonds
				tot++;
			}
		}
	}
// ------- Case 2: Alkyl Group -------------------

	int hy=0; // no of hypen
	int allen=0; // length of altemp
	if(isdigit(iupac[0]))   // change wont create problem
	{
		m=-1;
		for(i=0+halolen+cbhalo;iupac[i]!=root[noc][0];i++)
		{
			if(iupac[i]=='-')
			hy++;
			m++;
			altemp[m]=iupac[i];
		}
		if(hy==1)
		{
			k=-1;
			for(i=0+halolen+cbhalo;iupac[i]!='-';i++)
			{
				if(isdigit(iupac[i]))
				{
					k++;
					alloc[k]=iupac[i]-48;
				}
			}
			if(iupac[i]=='-')
			{
				for(j=0;j<=k;j++)
				{
					if(iupac[i+1]==alkyl[1][0])
					alknoc[j]=1;
					if(iupac[i+1]==alkyl[2][0])
					alknoc[j]=2;
					if(iupac[i+1]==alkyl[3][0])
					alknoc[j]=3;
					if(iupac[i+1]==alkyl[4][0])
					alknoc[j]=4;

				}
			}
		}

// ------- Multi Alkyl Group -----------
		else
		{
			allen=strlen(altemp);
			multibond=1;
			hy=0;
			k=-1;
			for(i=0;i<allen;i++)
			{
				if(altemp[i]=='-')
				{
					hy++;
					dashpos[hy-1]=i;
				}
				if(isdigit(altemp[i]))
				{
					k++;
					alloc[k]=altemp[i]-48;
				}
			}
			k=0;
			for(i=0;i<(hy+1)/2;i++)
			{
				if(altemp[dashpos[2*i]+1]==alkyl[1][0])
				alknoc[i]=1;
				else if(altemp[dashpos[2*i]+1]==alkyl[2][0])
				alknoc[i]=2;
				else if(altemp[dashpos[2*i]+1]==alkyl[3][0])
				alknoc[i]=3;
				else if(altemp[dashpos[2*i]+1]==alkyl[4][0])
				alknoc[i]=4;
			}

//---------- Assigning locatation in case of multi alkyl eg:1,1-methyl-----------------
			for(i=hy;i>=1;i-=2)
			{
				num=0;
				for(k=dashpos[i-1]+1;k>=0;k--)
				{
					if(isdigit(altemp[k]))
					{
						num++;
					}
				}
				if(altemp[dashpos[i-1]+1]==alkyl[1][0])
				cpy=1;
				else if(altemp[dashpos[i-1]+1]==alkyl[2][0])
				cpy=2;
				else if(altemp[dashpos[i-1]+1]==alkyl[3][0])
				cpy=3;
				else if(altemp[dashpos[i-1]+1]==alkyl[4][0])
				cpy=4;
				for(j=1;j<=num;j++)
				{
					alknoc[j-1]=cpy;
				}

			}
		}
	}

//---------- Halogen -------------------
	for(i=0;i<halolen;i++)
	{
		if(haltemp[i]=='-')
		{
			hhy++;
			hdashpos[hhy-1]=i;
		}
	}
	if(hhy==1)
	{
		for(i=0;i<halolen;i++)
		{
			if(isdigit(haltemp[i]))
			{
				htot++;
				halloc[htot-1]=haltemp[i]-48;
			}
		}
		for(i=0;i<htot;i++)
		{
			haltype[i]=haltemp[hdashpos[0]+1];
		}
	}
	else
	{
		// multi halo group
		htot=0;
		for(i=0;i<halolen;i++)
		{
			if(isdigit(haltemp[i]))
			{
				htot++;
				halloc[htot-1]=haltemp[i]-48;
			}
		}
		for(i=hhy;i>=1;i-=2)
		{
			hnum=0;
			for(k=hdashpos[i-1]+1;k>=0;k--)
			{
				if(isdigit(haltemp[k]))
				{
					hnum++;
				}
			}
			hcopy=haltemp[hdashpos[i-1]+1];   // reverse copy algorithm is used
			for(j=1;j<=hnum;j++)
			{
				haltype[j-1]=hcopy;
			}
		}
	}
	cout<<"";   //uncomment it if HHHHHHHHHH error appears

//---------------Functional group----------------------------

// ------------ Single Functional Group --------------
	k=-1;
	for(i=ilen+1;i<iupac.length();i++)     // Note:Don't Change iupac.length() to ilen
	{
		k++;
		functemp[k]=iupac[i];
		if(functemp[k]=='-')
		fdash=k;
		if(isdigit(functemp[k]))
		{
			ftot++;
			floc[ftot-1]=functemp[k]-48;
		}
	}
	funclen=strlen(functemp);
	for(i=0;i<=9;i++)
	{
		if(functemp[fdash+1]==fgroup[i][0]&&functemp[fdash+2]==fgroup[i][1]&&functemp[fdash+3]==fgroup[i][2]&&functemp[fdash+4]==fgroup[i][3]&&functemp[fdash+5]==fgroup[i][4])
		{
			fval=i;
		}
	}
	for(i=0;i<ftot;i++)
	{
		ftype[i]=fval;
	}
// ------------- Multiple Functional Group ------------
	prelen=newlen;
	if(cbhalo-2!=0&&funclen!=0)
	{
		for(i=0;i<prelen;i++)
		{
			if(isdigit(pretemp[i]))
			{
				pretot++;
				floc[ftot+pretot-1]=pretemp[i]-48;
			}
			if(pretemp[i]=='-')
			{
				prehy++;
				predashpos[prehy-1]=i;
			}
		}

			int precopy=0;
			int pretrue=0;
		if(prehy==1) // single prefix func group
		{
		 //char prefix[][20]={" ","Cyano","Hydroxy","Amino","Keto","Carboxy","Oxo","Carbamoyl"};
			for(i=1;i<=7;i++)
			{
				for(j=0;j<strlen(prefix[i]);j++)
				{
					if(pretemp[predashpos[0]+1+j]==prefix[i][j])
					{
						pretrue++;
					}
				}
				if(pretrue==strlen(prefix[i]))
				{
					precopy=i+1;
					break;
				}
				pretrue=0;
			}
			for(i=ftot;i<pretot+ftot;i++)
			{
				ftype[i]=precopy;
			}
		}
		else // multiple prefix func group
		{
			//------------
			int prenum;
			pretrue=0;
			for(i=prehy;i>=1;i-=2)
			{
				prenum=0;
				for(k=predashpos[i-1]+1;k>=0;k--)
				{
					if(isdigit(pretemp[k]))
					{
						prenum++;
					}
				}
				for(m=1;m<=7;m++)
				{
					pretrue=0;
					for(n=0;n<strlen(prefix[m]);n++)
					{
						if(pretemp[predashpos[i-1]+1+n]==prefix[m][n])
						pretrue++;
					}
					if(pretrue==strlen(prefix[m]))
					{
						precopy=m+1;
						break;
					}
				}
				for(j=ftot;j<prenum+ftot;j++)
				{
					ftype[j]=precopy;
					cout<<"";
				     //	cout<<"ftype:"<<ftype[j]<<endl;
				}
			}
		}
	}
	int correction=floc[0];   //wierd variabe, please dont touch.
	for(i=pretot+ftot;i<=20;i++)
	{
	       ftype[i]=-1;
	       floc[i]=-1;
	}
	floc[0]=correction;

//------- Passing values to structure ------------------
	for(i=0;i<10;i++)       // for alkyl group
	{
		an[i+1]=alknoc[i];
		pos[i+1]=alloc[i];
	}
	if(hhy==1)            // for single halo
	{
		for(i=0;i<htot;i++)
		{
			hal[i]=haltype[i];
			halp[9-i]=halloc[i];
		}
	}
	else if(hhy>=2)       // for multiple halo
	{
		for(i=0;i<htot;i++)
		{
			hal[i]=haltype[i];
			halp[9-i]=halloc[htot-i-1];
		}
	}
	hallen=i;
//------------- Structure ------------------
	system("cls");
//	initgraph(&gdriver, &gmode, "");
	setbkcolor(6);
	gotoxy(40-iupac.length()/2,20);
	cout<<iupac;
	gotoxy(40 - iupac.length()/2, 25);
	cout<<"1. Enter Again\n";
	gotoxy(40 - iupac.length()/2, 26);
	cout<<"2. View History";
	int choice;
	int a=240,b=186,c=179;
	char t=a,d=b,s=c;
	if(multi!=1)
	{
		if(check!=1)
		{
			for(i=1;i<=tot;i++)
			{
				bo[i]=bond[0];
			}
			for(i=0;i<tot;i++)
			{
				bp[i+1]=loc[i];
			}
		}
	}
	for(i=1;i<=noc;i++)
	{
		gotoxy(10+i*6,12);
		cout<<"C";
	}
	for(i=1;i<=(noc-1);i++)
	{
		gotoxy(13+i*6,12);
		cout<<"-";
	}
	for(i=1;i<=tot;i++)
	{
		gotoxy(13+bp[i]*6,12);
		if(bo[i]==2)
		{
			cout<<"=";
		}
		else if(bo[i]==3)
		{
			cout<<t;
		}
	}
// ------- Hydrogen -----------
for(i=1;i<=noc;i++)
		{
		gotoxy(10+i*6,11);
		cout<<s;
		gotoxy(10+i*6,13);
		cout<<s;
		gotoxy(10+i*6,10);
		cout<<"H";
		gotoxy(10+i*6,14);
		cout<<"H";
		}
	gotoxy(10,12);
	cout<<"H  -";
	gotoxy(13+noc*6,12);
	cout<<"-  H";

// ------------- H correction for multi bonds -----------
//Structure:Double and triple bond://////////////////////////////
	for(i=1;i<=9;i++)
	{
		gotoxy(13+bp[10-i]*6,12);
		if(bo[10-i]==2)
		{
			cout<<"=";
			gotoxy(16+bp[10-i]*6,11);
			cout<<" ";
			gotoxy(16+bp[10-i]*6,10);
			cout<<" ";
			gotoxy(10+bp[10-i]*6,11);
			cout<<" ";
			gotoxy(10+bp[10-i]*6,10);
			cout<<" ";
			for(k=0;k<=20;k++)
			{
				if(floc[k]==bp[10-i] && ftype[k]>=1 && ftype[k]<=8)
				{
					gotoxy(10+floc[k]*6,13);
					cout<<" ";
					gotoxy(10+floc[k]*6,14);
					cout<<" ";
				}

				else if(floc[k]==(bp[10-i]+1) && ftype[k]>=1 && ftype[k]<=8)
				{
					gotoxy(10+floc[k]*6,13);
					cout<<" ";
					gotoxy(10+floc[k]*6,14);
					cout<<" ";
				}
			}
			for(k=1;k<=9;k++)
			{
				if(bp[10-i]==(bp[10-k]+1))
				{
					gotoxy(10+bp[10-i]*6,13);
					cout<<" ";
					gotoxy(10+bp[10-i]*6,14);
					cout<<" ";
					gotoxy(16,13);
					cout<<s;
					gotoxy(16,14);
					cout<<"H";

				}
			}
		}else if(bo[10-i]==3)
		{
			cout<<t;
			gotoxy(16+bp[10-i]*6,11);
			cout<<" ";
			gotoxy(16+bp[10-i]*6,10);
			cout<<" ";
			gotoxy(10+bp[10-i]*6,11);
			cout<<" ";
			gotoxy(10+bp[10-i]*6,10);
			cout<<" ";
			gotoxy(16+bp[10-i]*6,13);
			cout<<" ";
			gotoxy(16+bp[10-i]*6,14);
			cout<<" ";
			gotoxy(10+bp[10-i]*6,13);
			cout<<" ";
			gotoxy(10+bp[10-i]*6,14);
			cout<<" ";
		}

	}
//structure:halogens//////////////////////////////////////////////////
		for(i=1;i<=9;i++){
			gotoxy(10+halp[10-i]*6,14);
			if(hal[hallen-i]=='f' || hal[hallen-i]=='F')
			{
				cout<<"F ";
				gotoxy(10+halp[10-i]*6,13);
				cout<<s;
			}else if(hal[hallen-i]=='c' || hal[hallen-i]=='C')
			{
				cout<<"Cl";
				gotoxy(10+halp[10-i]*6,13);
				cout<<s;
			}else if(hal[hallen-i]=='b' || hal[hallen-i]=='B')
			{
				cout<<"Br";
				gotoxy(10+halp[10-i]*6,13);
				cout<<s;
			}else if(hal[hallen-i]=='i' || hal[hallen-i]=='I')
			{
				cout<<"I ";
				gotoxy(10+halp[10-i]*6,13);
				cout<<s;
			}
		for(k=1;k<=9;k++)
		{
			if(halp[10-i]==halp[10-k] && (10-i)!=0 && (10-k)!=0 && (10-k)>(10-i))
			{
				gotoxy(10+halp[10-k]*6,10);
				if(hal[hallen-k]=='f' || hal[hallen-k]=='F')
				{
					cout<<"F ";
					gotoxy(10+halp[10-i]*6,11);
					cout<<s;
				}else if(hal[hallen-k]=='c' || hal[hallen-k]=='C')
				{
					cout<<"Cl";
					gotoxy(10+halp[10-i]*6,11);
					cout<<s;
				}else if(hal[hallen-k]=='b' || hal[hallen-k]=='B')
				{
					cout<<"Br";
					gotoxy(10+halp[10-i]*6,11);
					cout<<s;
				}else if(hal[hallen-k]=='i' || hal[hallen-k]=='I')
				{
					cout<<"I ";
					gotoxy(10+halp[10-i]*6,11);
					cout<<s;
				}
				//this is for chain ends(3-4 hals on 1 carbon)
				for(j=1;j<=9;j++){
					if(halp[10-i]==halp[10-j] && 10-j>10-k && 10-k!=0)
					{
						if(halp[10-i]==1)
						{
							gotoxy(9,12);
							if(hal[hallen-j]=='f' || hal[hallen-j]=='F')
							{
								cout<<"F   -";
							}else if(hal[hallen-j]=='c' || hal[hallen-j]=='C')
							{
								cout<<"Cl  -";
							}else if(hal[hallen-j]=='b' || hal[hallen-j]=='B')
							{
								cout<<"Br  -";
							}else if(hal[hallen-j]=='i' || hal[hallen-j]=='I')
							{
								cout<<"I   -";
							}

						}
						if(halp[10-i]==noc)
						{       if(noc==1)
							{
								hallen++;
							}
							gotoxy(13+noc*6,12);
							if(hal[hallen-j]=='f' || hal[hallen-j]=='F')
							{
								cout<<"-  F ";
							}else if(hal[hallen-j]=='c' || hal[hallen-j]=='C')
							{
								cout<<"-  Cl";
							}else if(hal[hallen-j]=='b' || hal[hallen-j]=='B')
							{
								cout<<"-  Br";
							}else if(hal[hallen-j]=='i' || hal[hallen-j]=='I')
							{
								cout<<"-  I ";
							}
							if(noc==1)
							{
								hallen--;
							}
						}
					}
				}
				for(l=0;l<20;l++)
				{
					if(floc[l]==halp[10-i] && ftype[l]>=1 && ftype[l]<=8)
					{
						if(floc[l]==1)
						{
							gotoxy(9,12);
							if(hal[hallen-i]=='f' || hal[hallen-i]=='F')
							{
								cout<<"F   -";
							}else if(hal[hallen-i]=='c' || hal[hallen-i]=='C')
							{
								cout<<"Cl  -";
							}else if(hal[hallen-i]=='b' || hal[hallen-i]=='B')
							{
								cout<<"Br  -";
							}else if(hal[hallen-i]=='i' || hal[hallen-i]=='I')
							{
								cout<<"I   -";
							}
						}
						if(floc[l]==noc)
						{
							gotoxy(13+noc*6,12);
							if(hal[hallen-i]=='f' || hal[hallen-i]=='F')
							{
								cout<<"-  F ";
							}else if(hal[hallen-i]=='c' || hal[hallen-i]=='C')
							{
								cout<<"-  Cl";
							}else if(hal[hallen-i]=='b' || hal[hallen-i]=='B')
							{
								cout<<"-  Br";
							}else if(hal[hallen-i]=='i' || hal[hallen-i]=='I')
							{
								cout<<"-  I ";
							}
						}
					}
				}
			}
		}
	}
//structure: alkyl substituents////////////////////////////////////////
	for(i=1;i<=9;i++)
	{

		for(j=1;j<=an[10-i];j++)
		{
			gotoxy(9+pos[10-i]*6,12+j*2);
			cout<<"HCH";
			gotoxy(10+pos[10-i]*6,11+j*2);
			cout<<s;
			if(pos[10-i]!=0)
			{
				gotoxy(10+pos[10-i]*6,13+an[10-i]*2);
				cout<<"H";
			}
		}
	}
	for(i=1;i<=9;i++)
	{
		for(k=1;k<=9;k++)
		{
			if(pos[10-i]==pos[k] && k!=10-i && i!=0 && k!=0)
			{
				if(an[k]>=an[10-i] && an[k]>0)
				{
					for(l=1;l<=an[10-i];l++)
					{
						gotoxy(9+pos[10-i]*6,12-l*2);
						cout<<"HCH";
						gotoxy(10+pos[10-i]*6,13-l*2);
						cout<<s;
					}
					gotoxy(10+pos[10-i]*6,13+an[k]*2);
					cout<<"H";
					gotoxy(10+pos[10-i]*6,11-an[10-i]*2);
					cout<<"H";
				}
				else if(an[10-i]>an[k] && an[10-i]>0)
				{
					for(l=1;l<=an[k];l++)
					{
						gotoxy(9+pos[k]*6,12-l*2);
						cout<<"HCH";
						gotoxy(10+pos[k]*6,13-l*2);
						cout<<s;
					}
					gotoxy(10+pos[10-i]*6,13+an[10-i]*2);
					cout<<"H";
					gotoxy(10+pos[k]*6,11-an[k]*2);
					cout<<"H";
					gotoxy(10+pos[10-i]*6,13+an[k]*2);
					cout<<s;
				}
			}
		}
	}


	//halogen alkyl overlap check///////////////////////////////////////
	for(i=1;i<=9;i++)
	{       for(k=1;k<=9;k++){
			  if(pos[10-i]==halp[10-k] && 10-i!=0 && 10-k!=0)
			  {
				gotoxy(10+halp[10-k]*6,10);
				if(hal[hallen-k]=='f' || hal[hallen-k]=='F')
				{
					cout<<"F ";
					gotoxy(16+halp[10-i]*6,11);
					cout<<s;
				}else if(hal[hallen-k]=='c' || hal[hallen-k]=='C')
				{
					cout<<"Cl";
					gotoxy(16+halp[10-i]*6,11);
					cout<<s;
				}else if(hal[hallen-k]=='b' || hal[hallen-k]=='B')
				{
					cout<<"Br";
					gotoxy(16+halp[10-i]*6,11);
					cout<<s;
				}else if(hal[hallen-k]=='i' || hal[hallen-k]=='I')
				{
					cout<<"I ";
					gotoxy(16+halp[10-i]*6,11);
					cout<<s;
				}
			}
		}
	}
//Functional group structure//////////////////////////////
	//This for loop is only for substituent type functional groups
	//(type 1,3,4,5)

	for(i=0;i<=20;i++)
	{      		if(ftype[i]>0 && floc[i]>=1)
			{
				if(ftype[i]==1)
				{
					gotoxy(8+floc[i]*6,10);
					cout<<"O=S=O";
					gotoxy(10+floc[i]*6,9);
					cout<<"O";
					gotoxy(10+floc[i]*6,8);
					cout<<"H";
					gotoxy(10+floc[i]*6,11);
					cout<<s;
				}
				else if(ftype[i]==3)
				{
					gotoxy(8+floc[i]*6,10);
					cout<<"  OH ";
					gotoxy(10+floc[i]*6,9);
					cout<<" ";
					gotoxy(10+floc[i]*6,8);
					cout<<" ";
					gotoxy(10+floc[i]*6,11);
					cout<<s;
				}
				else if(ftype[i]==4)
				{
					gotoxy(8+floc[i]*6,10);
					cout<<" HNH ";
					gotoxy(10+floc[i]*6,9);
					cout<<" ";
					gotoxy(10+floc[i]*6,8);
					cout<<" ";
					gotoxy(10+floc[i]*6,11);
					cout<<s;
				}
				else if(ftype[i]==5)
				{
					gotoxy(8+floc[i]*6,10);
					cout<<"  O  ";
					gotoxy(10+floc[i]*6,9);
					cout<<" ";
					gotoxy(10+floc[i]*6,8);
					cout<<" ";
					gotoxy(10+floc[i]*6,11);
					cout<<d;
					gotoxy(10+floc[i]*6,13);
					cout<<" ";
					gotoxy(10+floc[i]*6,14);
					cout<<" ";
				}
			for(k=0;k<=20;k++)
			{
				if(floc[i]==floc[k] && k<i)
				{
					if(ftype[k]==1)
					{
						gotoxy(8+floc[k]*6,14);
						cout<<"O=S=O";
						gotoxy(10+floc[k]*6,15);
						cout<<"O";
						gotoxy(10+floc[k]*6,16);
						cout<<"H";
						gotoxy(10+floc[k]*6,13);
						cout<<s;
					}
					else if(ftype[k]==3)
					{
						gotoxy(8+floc[k]*6,14);
						cout<<"  OH ";
						gotoxy(10+floc[k]*6,15);
						cout<<" ";
						gotoxy(10+floc[k]*6,16);
						cout<<" ";
						gotoxy(10+floc[k]*6,13);
						cout<<s;
					}
					else if(ftype[k]==4)
					{
						gotoxy(8+floc[k]*6,14);
						cout<<" HNH ";
						gotoxy(10+floc[k]*6,15);
						cout<<" ";
						gotoxy(10+floc[k]*6,16);
						cout<<" ";
						gotoxy(10+floc[k]*6,13);
						cout<<s;
					}
					else if(ftype[k]==5)
					{
						gotoxy(8+floc[k]*6,14);
						cout<<"  O  ";
						gotoxy(10+floc[k]*6,15);
						cout<<" ";
						gotoxy(10+floc[k]*6,16);
						cout<<" ";
						gotoxy(10+floc[k]*6,13);
						cout<<d;
						gotoxy(10+floc[k]*6,11);
						cout<<" ";
						gotoxy(10+floc[k]*6,10);
						cout<<" ";
				}
				//this is for chain ends(3-4 func grps on 1 carbon)
					for(j=0;j<=20;j++){
						if(floc[i]==floc[j] && j<k)
						{
							if(floc[i]==1)
							{
								if(ftype[j]==1)
								{
									gotoxy(7,12);
									cout<<"HO3S  -";
								}
								else if(ftype[j]==3)
								{
									gotoxy(7,12);
									cout<<"  HO  -";

								}
								else if(ftype[j]==4)
								{
									gotoxy(7,12);
									cout<<" H2N  -";
								}

							}
							else if(floc[i]==noc)
							{
								if(ftype[j]==1)
								{
									gotoxy(13+noc*6,12);
									cout<<"-  SO3H";
								}
								else if(ftype[j]==3)
								{
									gotoxy(13+noc*6,12);
									cout<<"-  OH  ";

								}
								else if(ftype[j]==4)
								{
									gotoxy(13+noc*6,12);
									cout<<"-  NH2 ";
								}

								if(noc==1)
								{
									if(ftype[j]==1)
									{
										gotoxy(13+noc*6,12);
										cout<<"-  SO3H";
									}
									else if(ftype[j]==3)
									{
										gotoxy(13+noc*6,12);
										cout<<"-  OH  ";

									}
									else if(ftype[j]==4)
									{
										gotoxy(13+noc*6,12);
										cout<<"-  NH2 ";
									}
								}
							}
						}
					}
				}
			}
		}
	}

	//this for loop is for terminating carbons, eg:COOH
	for(i=0;i<=20;i++)
	{
		if(floc[i]==1)
		{
			if(ftype[i]==7)
			{
				gotoxy(16,11);
				cout<<d;
				gotoxy(16,10);
				cout<<"O";
				gotoxy(16,13);
				cout<<" ";
				gotoxy(16,14);
				cout<<" ";
			}
			else if(ftype[i]==6)
			{
				gotoxy(16,11);
				cout<<d;
				gotoxy(16,10);
				cout<<"O";
				gotoxy(16,13);
				cout<<" ";
				gotoxy(16,14);
				cout<<" ";
				gotoxy(9,12);
				cout<<"HO";
			}else if(ftype[i]==2)
			{
				gotoxy(16,11);
				cout<<" ";
				gotoxy(16,10);
				cout<<" ";
				gotoxy(16,13);
				cout<<" ";
				gotoxy(16,14);
				cout<<" ";
				gotoxy(13,12);
				cout<<t;
				gotoxy(9,12);
				cout<<" N";
			}else if(ftype[i]==8)
			{
				gotoxy(16,11);
				cout<<d;
				gotoxy(16,10);
				cout<<"O";
				gotoxy(16,13);
				cout<<" ";
				gotoxy(16,14);
				cout<<" ";
				gotoxy(8,12);
				cout<<"H2N";
			}
		}else if(i!=20)
		{
			if(ftype[i]==7)
			{
				gotoxy(10+noc*6,11);
				cout<<d;
				gotoxy(10+noc*6,10);
				cout<<"O";
				gotoxy(10+noc*6,13);
				cout<<" ";
				gotoxy(10+noc*6,14);
				cout<<" ";
			}
			else if(ftype[i]==6)
			{
				gotoxy(10+noc*6,11);
				cout<<d;
				gotoxy(10+noc*6,10);
				cout<<"O";
				gotoxy(10+noc*6,13);
				cout<<" ";
				gotoxy(10+noc*6,14);
				cout<<" ";
				gotoxy(16+noc*6,12);
				cout<<"OH";
			}else if(ftype[i]==2&&i!=20)
			{
				gotoxy(10+noc*6,11);
				cout<<" ";
				gotoxy(10+noc*6,10);
				cout<<" ";
				gotoxy(10+noc*6,13);
				cout<<" ";
				gotoxy(10+noc*6,14);
				cout<<" ";
				gotoxy(13+noc*6,12);
				cout<<t;
				gotoxy(16+noc*6,12);
				cout<<"N";
			}else if(ftype[i]==8)
			{
				gotoxy(10+noc*6,11);
				cout<<d;
				gotoxy(10+noc*6,10);
				cout<<"O";
				gotoxy(10+noc*6,13);
				cout<<" ";
				gotoxy(10+noc*6,14);
				cout<<" ";
				gotoxy(16+noc*6,12);
				cout<<"NH2";
			}
		}
	}
	for(i=10;i<=14;i++)
	{
		gotoxy(2,i);
		cout<<"     ";
	}
	gotoxy(10,11);
	cout<<" ";
	gotoxy(40 - iupac.length()/2, 27);
	cin>>choice;
	switch(choice){
		case 1: formula();
				break;
		case 2: system("cls");
				DecodeString();
				break;
	}
	getch();
	closegraph();
//-----------------
} // func braces


void brain()
{
	mainset:
	system("cls");
	cleardevice();
//	initgraph(&gd, &gm, "");
//	setbkcolor(15);
	setcolor(WHITE);
	map_iupac();
	gotoxy(28,4);
	settextjustify(CENTER_TEXT, CENTER_TEXT);
	settextstyle(6,0,4);
	outtextxy(600,300, "Know Your Compound!!!");
	cout<<"Know Your Compound!";
	gotoxy(22,6);
	outtextxy(600,400, "1. IUPAC Naming for structural Formulae");
	cout<<"1.IUPAC Naming for Structural Formulae";
	gotoxy(22,7);
	cout<<"2.Help";
	outtextxy(600,500, "2. Help");
	gotoxy(22,8);
	cout<<"3.Exit";
	outtextxy(600, 600, "3. Exit");
	opset:
	char op=getch();
	setbkcolor(BLACK);
	char helpop;
	if(op=='1')
	{
		formula();
		goto mainset;
	}
	else if(op=='2')
	{
		helpstart:
		help_iupac();
		helpopset:
		helpop=getch();
		if(helpop=='b'||helpop=='B')
		goto mainset;
		else if(helpop=='f'||helpop=='F')
		{
			system("cls");
			map_iupac();
			gotoxy(19,4);
			cout<<"Name";
			gotoxy(36,4);
			cout<<"Prefix";
			gotoxy(53,4);
			cout<<"Suffix";
			for(int i=4;i<=23;i++)
			{
				gotoxy(30,i);
				cout<<char(179);
				gotoxy(48,i);
				cout<<char(179);
			}
			for(int i=12;i<=64;i++)
			{
				gotoxy(i,3);
				cout<<"-";
				gotoxy(i,5);
				cout<<"-";
			}
			gotoxy(17,6);
			cout<<"1.Sulphonic";
			gotoxy(20,7);
			cout<<"acid";
			gotoxy(38,6);
			cout<<"-";
			gotoxy(53,6);
			cout<<"sulphonic";
			gotoxy(53,7);
			cout<<"acid";

			gotoxy(17,9);
			cout<<"2.Carboxylic";
			gotoxy(19,10);
			cout<<"acid";
			gotoxy(36,9);
			cout<<"Carboxy";
			gotoxy(53,9);
			cout<<"oic acid";

			gotoxy(17,12);
			cout<<"3.Amide";
			gotoxy(36,12);
			cout<<"Carbamoyl";
			gotoxy(53,12);
			cout<<"amide";

			gotoxy(17,14);
			cout<<"4.Cyanide";
			gotoxy(36,14);
			cout<<"Cyano";
			gotoxy(53,14);
			cout<<"nitrile";

			gotoxy(17,16);
			cout<<"5.Aldehyde";
			gotoxy(36,16);
			cout<<"Oxo";
			gotoxy(53,16);
			cout<<"al";

			gotoxy(17,18);
			cout<<"6.Ketone";
			gotoxy(36,18);
			cout<<"Keto";
			gotoxy(53,18);
			cout<<"one";

			gotoxy(17,20);
			cout<<"7.Alcohol";
			gotoxy(36,20);
			cout<<"Hydroxy";
			gotoxy(53,20);
			cout<<"ol";

			gotoxy(17,22);
			cout<<"8.Amine";
			gotoxy(36,22);
			cout<<"Amino";
			gotoxy(53,22);
			cout<<"amine";
			getch();
			goto helpstart;///-------------
		}
		else
		goto helpopset;

	}
	else if(op=='3')
	{
		initgraph(&gdriver, &gmode, "");
		settextjustify(CENTER_TEXT, CENTER_TEXT);
		setbkcolor(0);

			cout<<"";
			closegraph();
	}
	else
	goto opset;
	closegraph();
}




//---------  END OF FUNCTION ----------------
//---------- END OF KNOW YOUR COMPOUND ------------
int main()
{
	initgraph(&gdriver, &gmode, "");
	//initgraph(&gdriver, &gmode, "c:\\tc\\bgi");
	errorcode = graphresult();
	settextjustify(CENTER_TEXT, CENTER_TEXT);
	int x=600,y=300;
	cleardevice();
	setcolor(WHITE);
	settextstyle(6,0,4);
	for(int i=1;i<=40;i++)
	{
		cleardevice();
		setbkcolor(12);
		outtextxy(x,y-i,"Welcomme to know your compund!");
		delay(100);
	}
	delay(500);
	settextstyle(6,0,4);
	outtextxy(x,y+50,"- Developed by:");
	outtextxy(x,y+100,"Chirag Satapathy - 19BEI0107");
	outtextxy(x,y+150,"Ritik Kumar - 19BCI0079");
	outtextxy(x,y+200,"Muskan Agarwal - 19BCI0067");
	outtextxy(x,y+250,"Alka Rani - 19BCI0004");
	outtextxy(x,y+300,"Harsh Tyagi - 19BCI0063");
	delay(600);
	settextstyle(5,0,3);
	outtextxy(x+400,y+400,"Press any key to continue ...........");
	getch();
//	//-- Acknowledgement Page
//	cleardevice();
//	setcolor(WHITE);
//	setbkcolor(8);
//	settextstyle(3,0,5);
//	outtextxy(x,20,"Acknowledgement");
//	delay(500);
//	settextstyle(6,0,3);
//	outtextxy(x,y-80," I would like to thank our Computer Teacher RASHMI MAA'M for providing me  ");
//	delay(300);
//	outtextxy(x,y-50," with this wonderful opportunity .");
//	//
//	delay(300);
//	//outtextxy(x,y-20," I would also like to thank Rahul Haridas (Class 11 A) for helping");
//	//delay(300);
//	//outtextxy(x,y+10," me with structures in the IUPAC project .");
//	settextstyle(5,0,3);
//	delay(500);
//	outtextxy(x+100,y+150,"Press any key to continue........");
//	getch();
	int brainval=0;
	intromain:
	cleardevice();
	setbkcolor(9);
	settextstyle(1,0,6);
//	outtextxy(x,20,"NOTHING WORTH HAVING COME EASY");
//	delay(500);
//	settextstyle(7,0,5);
//	outtextxy(x-95,90, "1. Olympic Medals");
//	delay(500);
	outtextxy(x+14,140,"1. Know Your Compound");
	delay(500);
	outtextxy(x-196,190,"2. Exit");
	delay(500);
//	char introop=' ';
	char introop = ' ';
	introset:
	introop = getch();
//	if(introop=='1')
//	{
//		closegraph();
//		delay(100);
//		brainval=1;
//		goto intromain;
//	}
	if(introop=='1')
	{
//		closegraph();
		brain();
		delay(100);
		brainval=1;
		goto intromain;
	}
	else if(introop=='2')
	{
		cleardevice();
		setbkcolor(BLACK);
		settextstyle(2,0,20);
		outtextxy(x,y,"THANK YOU");
		getch();
		cleardevice();
		setbkcolor(0);
		outtextxy(x,y+89," ");
		closegraph();
		exit(0);
	}
	else
	{
		goto introset;

	}
	cleardevice();
	setbkcolor(0);
	outtextxy(x,y+89," ");
	closegraph();
	return 0;
}
