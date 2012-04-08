//STL HEADER
struct EBU_GSI{
	unsigned char CPN[3];
	unsigned char DFC[8];
	unsigned char DSC;
	unsigned char CCT[2];
	unsigned char LC[2];
	unsigned char OPT[32];
	unsigned char OET[32];
	unsigned char TPT[32];
	unsigned char TET[32];
	unsigned char TN[32];
	unsigned char TCD[32];
	unsigned char SLR[16];
	unsigned char CD[6];
	unsigned char RD[6];
	unsigned char RN[2];
	unsigned char TNB[5]; // Number of TTI Blocks
	unsigned char TNS[5];
	unsigned char TNG[3];
	unsigned char MNC[2];
	unsigned char MNR[2];
	unsigned char TCS[1];
	unsigned char TCP[8]; //Timecode formatted as HHMMSSFF
	unsigned char TCF[8]; //idem
	unsigned char TND;
	unsigned char DSN;
	unsigned char CO[3];
	unsigned char PUB[32];
	unsigned char EN[32];
	unsigned char ECD[32];
	unsigned char spare[75];
	unsigned char UDA[576];
};

//STL TIMECODE
struct EBU_TC{
	unsigned char hours;
	unsigned char minutes;
	unsigned char seconds;
	unsigned char frames;
};

//STL SUBTITLE BLOCK
struct EBU_TTI{
	unsigned char SGN;
	unsigned char SN[2];
	unsigned char EBN;
	unsigned char CS;
	struct EBU_TC TCI;
	struct EBU_TC TCO;
	unsigned char VP;
	unsigned char JC;
	unsigned char CF;
	unsigned char TF[112];
};

struct EBU{
	struct EBU_GSI gsi;
	struct EBU_TTI* tti;
};

struct EBU parseEBU(FILE* f);
void saveEBU(FILE* f, const struct EBU* ebu);
struct EBU_TC* charToTC(const unsigned char TC[8]);
void TCToChar(unsigned char tc[8],const struct EBU_TC TC);
struct EBU_TC shiftTC(const struct EBU_TC tc, const struct EBU_TC shift, const short int positive);
void shiftTCs(struct EBU* ebu, const struct EBU_TC shift, const int positive);