typedef struct protasi{
	char line[128];
	int useful;
	int procid;
}protasi;


typedef struct shared {
	protasi inds[10];
	protasi outds[10];
	int pidmatch;
}shared;
