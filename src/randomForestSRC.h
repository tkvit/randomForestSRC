////**********************************************************************
////**********************************************************************
////
////  RANDOM FORESTS FOR SURVIVAL, REGRESSION, AND CLASSIFICATION (RF-SRC)
////  Version 2.2.0 (bld20160516)
////
////  Copyright 2016, University of Miami
////
////  This program is free software; you can redistribute it and/or
////  modify it under the terms of the GNU General Public License
////  as published by the Free Software Foundation; either version 3
////  of the License, or (at your option) any later version.
////
////  This program is distributed in the hope that it will be useful,
////  but WITHOUT ANY WARRANTY; without even the implied warranty of
////  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
////  GNU General Public License for more details.
////
////  You should have received a copy of the GNU General Public
////  License along with this program; if not, write to the Free
////  Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
////  Boston, MA  02110-1301, USA.
////
////  ----------------------------------------------------------------
////  Project Partially Funded By: 
////  ----------------------------------------------------------------
////  Dr. Ishwaran's work was funded in part by DMS grant 1148991 from the
////  National Science Foundation and grant R01 CA163739 from the National
////  Cancer Institute.
////
////  Dr. Kogalur's work was funded in part by grant R01 CA163739 from the 
////  National Cancer Institute.
////  ----------------------------------------------------------------
////  Written by:
////  ----------------------------------------------------------------
////    Hemant Ishwaran, Ph.D.
////    Director of Statistical Methodology
////    Professor, Division of Biostatistics
////    Clinical Research Building, Room 1058
////    1120 NW 14th Street
////    University of Miami, Miami FL 33136
////
////    email:  hemant.ishwaran@gmail.com
////    URL:    http://web.ccs.miami.edu/~hishwaran
////    --------------------------------------------------------------
////    Udaya B. Kogalur, Ph.D.
////    Adjunct Staff
////    Department of Quantitative Health Sciences
////    Cleveland Clinic Foundation
////    
////    Kogalur & Company, Inc.
////    5425 Nestleway Drive, Suite L1
////    Clemmons, NC 27012
////
////    email:  ubk@kogalur.com
////    URL:    http://www.kogalur.com
////    --------------------------------------------------------------
////
////**********************************************************************
////**********************************************************************


#include <R_ext/Print.h>
#include <Rdefines.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <time.h>
#ifdef _OPENMP
#include           <omp.h>
#endif
#ifndef NULL
#define NULL 0
#endif
#ifndef TRUE
#define TRUE      0x01
#endif
#ifndef FALSE
#define FALSE     0x00
#endif
#define RF_OUTP_ID   0  
#define RF_STRG_ID   1  
#define RF_FSRG_ID   2  
#define RF_OSRG_ID   3  
#define RF_FCIF_ID   4  
#define RF_OCIF_ID   5  
#define RF_FSRV_ID   6  
#define RF_OSRV_ID   7  
#define RF_FMRT_ID   8  
#define RF_OMRT_ID   9  
#define RF_FCLS_ID  10  
#define RF_OCLS_ID  11  
#define RF_FRGR_ID  12  
#define RF_ORGR_ID  13  
#define RF_ER_SURV  14  
#define RF_ER_CLAS  15  
#define RF_ER_REGR  16  
#define RF_PROX_ID  17  
#define RF_LEAF_ID  18  
#define RF_TREE_ID  19  
#define RF_NODE_ID  20  
#define RF_PARM_ID  21  
#define RF_CONT_PT  22  
#define RF_MWCP_SZ  23  
#define RF_MWCP_PT  24  
#define RF_SEED_ID  25  
#define RF_VMP_SRG  26  
#define RF_VMP_CLS  27  
#define RF_VMP_RGR  28  
#define RF_MISS_ID  29  
#define RF_OMIS_ID  30  
#define RF_VUSE_ID  31  
#define RF_DPTH_ID  32  
#define RF_NMBR_ID  33  
#define RF_BMBR_ID  34  
#define RF_SPLT_ST  35  
#define RF_SPLT_VR  36  
#define RF_PMBR_ID  37  
#define RF_WGHT_ID  38  
#define RF_TN_SURV  39  
#define RF_TN_MORT  40  
#define RF_TN_NLSN  41  
#define RF_TN_CSHZ  42  
#define RF_TN_CIFN  43  
#define RF_TN_REGR  44  
#define RF_TN_CLAS  45  
#define RF_TN_MCNT  46  
#define RF_USPV_ST  47  
#define RF_MTRY_ID  48  
#define RF_MTRY_ST  49  
#define RF_MWCP_CT  50  
#define RF_SEXP_CNT 51  
#define SEXP_TYPE_NUMERIC 0
#define SEXP_TYPE_INTEGER 1
#define OPT_FENS       0x000001  
#define OPT_OENS       0x000002  
#define OPT_PERF       0x000004  
#define OPT_USPV_STAT  0x000008  
#define OPT_LEAF       0x000010  
#define OPT_TREE       0x000020  
#define OPT_SEED       0x000040  
#define OPT_MISS       0x000080  
#define OPT_VIMP_TYP1  0x000100  
#define OPT_VIMP_TYP2  0x000200  
#define OPT_VIMP_JOIN  0x000400  
#define OPT_MEMB       0x000800  
#define OPT_VUSE_TYPE  0x001000  
#define OPT_VUSE       0x002000  
#define OPT_REST       0x004000  
#define OPT_PERF_CALB  0x008000  
#define OPT_IMPU_ONLY  0x010000  
#define OPT_OUTC_TYPE  0x020000  
#define OPT_SPLT_NULL  0x040000  
#define OPT_BOOT_NODE  0x080000  
#define OPT_BOOT_NONE  0x100000  
#define OPT_COMP_RISK  0x200000  
#define OPT_SPLDPTH_F  0x400000  
#define OPT_SPLDPTH_T  0x800000  
#define OPT_VIMP_LEOB  0x1000000 
#define OPT_VIMP       0x2000000 
#define OPT_PROX_MARG  0x4000000 
#define OPT_NODE_STAT  0x8000000 
#define OPT_PROX      0x10000000 
#define OPT_PROX_IBG  0x20000000 
#define OPT_PROX_OOB  0x40000000 
#define OPT_PROX_FUL  0x60000000 
#define OPT_WGHT      0x00000001 
#define OPT_WGHT_TYP1 0x00000002 
#define OPT_WGHT_TYP2 0x00000004 
#define OPT_MISS_SKIP 0x00000010 
#define OPT_TERM      0x00000040 
#define OPT_TERM_MEMB 0x00000080 
#define OPT_SPLT_CUST 0x00000F00 
#define OPT_BOOT_SWOR 0x00001000 
#define OPT_TREE_ERR  0x00002000 
#define ACTIVE    0x02
#define LEFT      0x01
#define RIGHT     0x00
#define NEITHER   0x03
#define EPSILON 1.0e-9
#define RF_GROW   0x01
#define RF_PRED   0x02
#define RF_REST   0x04
#define RF_PART   0x08
#define SURV_LGRNK   1
#define SURV_LRSCR   2
#define SURV_CR_LAU  3
#define SURV_CR_LOG  4
#define RAND_SPLIT   5
#define REGR_WT_NRM  6
#define REGR_WT_OFF  7
#define REGR_WT_HVY  8
#define CLAS_WT_NRM  9
#define CLAS_WT_OFF 10
#define CLAS_WT_HVY 11
#define USPV_SPLIT  12
#define MVRG_SPLIT  13 
#define MVCL_SPLIT  14 
#define CUST_SPLIT  15
#define MAXM_SPLIT  15 
#define CLAS_FAM     0
#define REGR_FAM     1
#define SURV_FAM     2
#define CRSK_FAM     3
#define APROX 0
#define EXACT 1
#define SIZE_OF_INTEGER sizeof(uint)
#define MAX_EXACT_LEVEL SIZE_OF_INTEGER * 8
#define SAFE_FACTOR_SIZE 16
#define MARGINAL_SIZE 4
#define RF_WGHT_UNIFORM 1
#define RF_WGHT_INTEGER 2
#define RF_WGHT_GENERIC 3
typedef unsigned int   uint;
#define RFprintf Rprintf
typedef struct node Node;
struct node {
  struct node *parent;
  unsigned int xSize;
  char splitFlag;
  unsigned int splitParameter;
  double splitValueCont;
  double splitStatistic;
  double mean;
  double variance;
  unsigned int *urStat;
  unsigned int  urStatSize;
  unsigned int  mtrySize;
  unsigned int *mtryIndx;
  double       *mtryStat;
  unsigned int splitValueFactSize;
  unsigned int *splitValueFactPtr;
  unsigned int nodeID;
  unsigned int depth;
  char pseudoTerminal;
  struct terminal *mate;
  struct node *left;
  struct node *right;
  char *permissibleSplit;
  unsigned int *permissibleSplitIndex;
  unsigned int  permissibleSizeAlloc;
  unsigned int  permissibleSizeActual;
  unsigned int *splitDepth;
  unsigned int mpIndexSize;
  unsigned int fmpIndexSize;
  int *mpSign;
  int *fmpSign;
  char imputed;
  unsigned int *lmpIndex;
  unsigned int  lmpIndexAllocSize, lmpIndexActualSize;
  double *lmpValue;
  unsigned int *flmpIndex;
  unsigned int  flmpIndexAllocSize, flmpIndexActualSize;
  double *flmpValue;
};
typedef struct terminal Terminal;
struct terminal {
  unsigned int nodeID;
  unsigned int *lmiIndex;
  unsigned int  lmiAllocSize, lmiSize;
  double       *lmiValue;
  unsigned int eTypeSize;
  unsigned int mTimeSize;
  unsigned int eTimeSize;
  unsigned int sTimeSize;
  unsigned int *atRiskCount;
  unsigned int **eventCount;
  unsigned int *eventTimeIndex;
  double **localRatio;
  double **localCSH;
  double **localCIF;
  double *localSurvival;
  double *localNelsonAalen;
  double **CSH;
  double **CIF;
  double *survival;
  double *nelsonAalen;
  double *mortality;
  unsigned int   rnfCount;
  double        *meanResponse;
  unsigned int   rfCount;
  unsigned int  *rfSize;
  unsigned int **multiClassProb;
  double        *maxClass;
  double weight;
  unsigned int membrCount;
};
typedef struct factor Factor;
struct factor {
  unsigned int r; 
  unsigned int cardinalGroupCount; 
  void *complementaryPairCount;
  void *cardinalGroupSize; 
  unsigned int **cardinalGroupBinary;
  unsigned int mwcpSize;
};
void randomStack(uint bSize, uint xSize);
void randomUnstack(uint bSize, uint xSize);
void randomSetChainParallel(uint b, int value);
void randomSetUChainParallel(uint b, int value);
void randomSetUChainParallelCov(uint b, int value);
void randomSetChainSerial(uint b, int value);
void randomSetUChainSerial(uint b, int value);
void randomSetUChainSerialCov(uint b, int value);
int randomGetChainParallel(uint b);
int randomGetUChainParallel(uint b);
int randomGetUChainParallelCov(uint b);
int randomGetChainSerial(uint b);
int randomGetUChainSerial(uint b);
int randomGetUChainSerialCov(uint b);
float randomChainParallel(uint b);
float randomUChainParallel(uint b);
float randomUChainParallelCov(uint b);
float randomChainSerial(uint b);
float randomUChainSerial(uint b);
float randomUChainSerialCov(uint b);
float ran1_generic(int *iy, int *iv, int *idum);
void lcgenerator(unsigned int *seed, unsigned char reset);
float ran1_original(int *idum);
enum alloc_type{
  NRUTIL_DPTR,   
  NRUTIL_UPTR,   
  NRUTIL_IPTR,   
  NRUTIL_CPTR,   
  NRUTIL_NPTR,   
  NRUTIL_TPTR,   
  NRUTIL_FPTR,   
  NRUTIL_DPTR2,  
  NRUTIL_UPTR2,  
  NRUTIL_IPTR2,  
  NRUTIL_CPTR2,  
  NRUTIL_NPTR2,  
  NRUTIL_TPTR2,  
  NRUTIL_FPTR2,  
  NRUTIL_DPTR3,  
  NRUTIL_UPTR3,  
  NRUTIL_NPTR3,  
  NRUTIL_DPTR4,  
  NRUTIL_UPTR4   
};
unsigned int upower (unsigned int x, unsigned int n);
unsigned int upower2 (unsigned int n);
unsigned int ulog2 (unsigned int n);
void hpsort(double *ra, unsigned int n);
void hpsortui(unsigned int *ra, unsigned int n);
void sort(double *arr, unsigned int n);
void indexx(unsigned int n, double *arr, unsigned int *indx);
void nrerror(char error_text[]);
void *gblock(size_t size);
void free_gblock(void *v, size_t size);
void *gvector(unsigned int nl, unsigned int nh, size_t size);
void free_gvector(void *v, unsigned int nl, unsigned int nh, size_t size);
char *cvector(unsigned int nl, unsigned int nh);
void free_cvector(char *v, unsigned int nl, unsigned int nh);
char **cmatrix(unsigned int nrl, unsigned int nrh, unsigned int ncl, unsigned int nch);
void free_cmatrix(char **v, unsigned int nrl, unsigned int nrh, unsigned int ncl, unsigned int nch);
int *ivector(unsigned int nl, unsigned int nh);
void free_ivector(int *v, unsigned int nl, unsigned int nh);
int **imatrix(unsigned int nrl, unsigned int nrh, unsigned int ncl, unsigned int nch);
void free_imatrix(int **v, unsigned int nrl, unsigned int nrh, unsigned int ncl, unsigned int nch);
unsigned int *uivector(unsigned int nl, unsigned int nh);
void free_uivector(unsigned int *v, unsigned int nl, unsigned int nh);
unsigned int **uimatrix(unsigned int nrl, unsigned int nrh, unsigned int ncl, unsigned int nch);
void free_uimatrix(unsigned int **v, unsigned int nrl, unsigned int nrh, unsigned int ncl, unsigned int nch);
double *dvector(unsigned int nl, unsigned int nh);
void free_dvector(double *v, unsigned int nl, unsigned int nh);
double **dmatrix(unsigned int nrl, unsigned int nrh, unsigned int ncl, unsigned int nch);
void free_dmatrix(double **v, unsigned int nrl, unsigned int nrh, unsigned int ncl, unsigned int nch);
double ***dmatrix3(unsigned int n3l, unsigned int n3h, unsigned int nrl, unsigned int nrh, unsigned int ncl, unsigned int nch);
void free_dmatrix3(double ***v, unsigned int n3l, unsigned int n3h, unsigned int nrl, unsigned int nrh, unsigned int ncl, unsigned int nch);
double ****dmatrix4(unsigned int n4l, unsigned int n4h, unsigned int n3l, unsigned int n3h, unsigned int nrl, unsigned int nrh, unsigned int ncl, unsigned int nch);
void free_dmatrix4(double ****v, unsigned int n4l, unsigned int n4h, unsigned int n3l, unsigned int n3h, unsigned int nrl, unsigned int nrh, unsigned int ncl, unsigned int nch);
void *new_vvector(unsigned int nl, unsigned int nh, enum alloc_type type);
void free_new_vvector(void *v, unsigned int nl, unsigned int nh, enum alloc_type type);
void nrCopyMatrix(
  unsigned int **new,
  unsigned int **old,
  unsigned int nrow,
  unsigned int ncol
);
void nrCopyVector(
  char *new,
  char *old,
  unsigned int ncol
);
Node *makeNode(unsigned int xSize, unsigned int permissibleSizeAlloc, unsigned int urStatSize, unsigned int mtrySize);
void freeNode(Node *parent);
void getNodeInfo(Node *leaf);
void setParent(
  Node *daughter,
  Node *parent
);
void setLeftDaughter(
   Node *daughter,
   Node *parent
);
void setRightDaughter(
  Node *daughter,
  Node *parent
);
char forkNode(Node         *parent,
              unsigned int  splitParameter,
              double        splitValueMaxCont,
              unsigned int  splitValueMaxFactSize,
              unsigned int *splitValueMaxFactPtr);
void stackMPSign(Node *node, unsigned int mpIndexSize);
void unstackMPSign(Node *node);
void stackFMPSign(Node *node, unsigned int fmpIndexSize);
void unstackFMPSign(Node *node);
void stackNodeLMPIndex(Node *node, unsigned int size);
void unstackNodeLMPIndex(Node *node);
void stackNodeFLMPIndex(Node *node, unsigned int size);
void unstackNodeFLMPIndex(Node *node);
void stackSplitDepth(Node *tNode, unsigned int depth);
void unstackSplitDepth(Node *tNode);
Terminal *makeTerminal();
void freeTerminal(Terminal *parent);
void stackTermLMIIndex(Terminal *tTerm, unsigned int size);
void unstackTermLMIIndex(Terminal *tTerm);
void freeTerminalNodeLocalSurvivalStructures(Terminal *tTerm);
void freeTerminalNodeSurvivalStructuresNonVimp(Terminal *tTerm);
void freeTerminalNodeSurvivalStructuresFinal(Terminal *tTerm);
void freeTerminalNodeNonSurvivalStructures(Terminal *tTerm);
void stackAtRiskAndEventCounts(Terminal *tTerm, unsigned int eTypeSize, unsigned int mTimeSize);
void stackEventTimeIndex(Terminal *tTerm, unsigned int mTimeSize);
void unstackAtRiskAndEventCounts(Terminal *tTerm);
void unstackEventTimeIndex(Terminal *tTerm);
void unstackAtRisk(Terminal *tTerm);
void stackLocalRatio(Terminal *tTerm, unsigned int eTypeSize, unsigned int eTimeSize);
void unstackLocalRatio(Terminal *tTerm);
void stackLocalSurvival(Terminal *tTerm, unsigned int eTimeSize);
void unstackLocalSurvival(Terminal *tTerm);
void stackLocalNelsonAalen(Terminal *tTerm, unsigned int eTimeSize);
void unstackLocalNelsonAalen(Terminal *tTerm);
void stackLocalCSH(Terminal *tTerm, unsigned int eTypeSize, unsigned int eTimeSize);
void unstackLocalCSH(Terminal *tTerm);
void stackLocalCIF(Terminal *tTerm, unsigned int eTypeSize, unsigned int eTimeSize);
void unstackLocalCIF(Terminal *tTerm);
void stackNelsonAalen(Terminal *tTerm, unsigned int sTimeSize);
void unstackNelsonAalen(Terminal *tTerm);
void stackSurvival(Terminal *tTerm, unsigned int sTimeSize);
void unstackSurvival(Terminal *tTerm);
void stackCSH(Terminal *tTerm, unsigned int eTypeSize, unsigned int sTimeSize);
void unstackCSH(Terminal *tTerm);
void stackCIF(Terminal *tTerm, unsigned int eTypeSize, unsigned int sTimeSize);
void unstackCIF(Terminal *tTerm);
void stackMortality(Terminal *tTerm, unsigned int eTypeSize);
void unstackMortality(Terminal *tTerm);
void stackMultiClassProb(Terminal *tTerm, unsigned int rfCount, unsigned int *rfSize);
void unstackMultiClassProb(Terminal *tTerm);
void stackMeanResponse(Terminal *tTerm, unsigned int rnfCount);
void unstackMeanResponse(Terminal *tTerm);
void getTerminalInfo(Terminal *termPtr);
Factor *makeFactor(uint r, char bookFlag);
void free_Factor(Factor *f);
char bookFactor(Factor *f);
char unbookFactor(Factor *f);
void bookPair (uint    levelCount, 
               uint    setSize, 
               uint    setColumn, 
               uint   *setRow, 
               uint   *pair, 
               Factor *f);
void nChooseK (uint n, uint r, char type, void *result);
char reduceFraction(uint *numerator, uint *denominator);
char splitOnFactor(uint level, uint *mwcp);
void initializeCDF(uint     treeID,
                   uint    *permissibilityIndex,
                   char    *permissibilityFlag,
                   uint     permissibilitySize,
                   uint     weightType,
                   double  *weight,
                   uint    *weightSorted,
                   uint     maxDensitySize,
                   uint   **index,
                   uint    *sampleSize,
                   double **cdf,
                   uint    *cdfSize,
                   uint   **cdfSort,
                   uint   **density,
                   uint    *densitySize,
                   uint  ***densitySwap);
void updateCDF(uint    treeID,
               uint    weightType,
               double *weight,
               uint   *index,
               uint   *sampleSize,
               uint    sampleSlot,
               double *cdf,
               uint   *cdfSize,
               uint   *density,
               uint   *densitySize,
               uint  **densitySwap,
               uint    absoluteSlot);
uint sampleFromCDF (float (*genericGenerator) (uint),
                    uint    treeID,
                    uint    weightType,
                    uint   *sampleIndex,
                    uint    sampleSize,
                    uint   *sampleSlot,
                    double *cdf,
                    uint    cdfSize,
                    uint   *cdfSort,
                    uint   *density,
                    uint    densitySize);
void discardCDF(uint     treeID,
                uint     permissibilitySize,
                uint     weightType,
                double  *weight,
                uint     maxDensitySize,
                uint    *index,
                uint    *density,
                uint   **densitySwap,
                double  *cdf,
                uint    *cdfSort);
uint sampleUniformlyFromVector (uint    treeID,
                                uint   *index,
                                uint    size,
                                uint   *sampleSlot);
SEXP rfsrcGrow(SEXP traceFlag,
               SEXP seedPtr,
               SEXP opt,
               SEXP optHigh,
               SEXP splitRule,
               SEXP splitRandomCount,
               SEXP randomCovariateCount,
               SEXP randomResponseCount,
               SEXP minimumNodeSize,
               SEXP maximumNodeDepth,
               SEXP crWeight,
               SEXP forestSize,
               SEXP observationSize,
               SEXP rSize,
               SEXP rType,
               SEXP rLevels,
               SEXP rData,
               SEXP xSize,
               SEXP xType,
               SEXP xLevels,
               SEXP bootstrapSize,
               SEXP caseWeight,
               SEXP xWeight,
               SEXP xData,
               SEXP timeInterestSize,
               SEXP timeInterest,
               SEXP missTree,
               SEXP imputeSize,
               SEXP numThreads);
SEXP rfsrcPredict(SEXP traceFlag,
                  SEXP seedPtr,
                  SEXP opt,
                  SEXP optHigh,
                  SEXP forestSize,
                  SEXP observationSize,
                  SEXP rSize,
                  SEXP rType,
                  SEXP rTarget,
                  SEXP rTargetCount,
                  SEXP rLevels,
                  SEXP rData,
                  SEXP xSize,
                  SEXP xType,
                  SEXP xLevels,
                  SEXP xData,
                  SEXP bootstrapSize,
                  SEXP caseWeight,
                  SEXP ptnCount,
                  SEXP sobservationSize,
                  SEXP sobservationIndv,
                  SEXP fobservationSize,
                  SEXP frSize,
                  SEXP frData,
                  SEXP fxData,
                  SEXP timeInterestSize,
                  SEXP timeInterest,
                  SEXP treeID,
                  SEXP nodeID,
                  SEXP parmID,
                  SEXP contPT,
                  SEXP mwcpSZ,
                  SEXP mwcpPT,
                  SEXP tnSURV,
                  SEXP tnMORT,
                  SEXP tnNLSN,
                  SEXP tnCSHZ,
                  SEXP tnCIFN,
                  SEXP tnREGR,
                  SEXP tnCLAS,
                  SEXP tnMCNT,
                  SEXP tnMEMB,
                  SEXP totalNodeCount,
                  SEXP seed,
                  SEXP intrPredictorSize,
                  SEXP intrPredictor,
                  SEXP numThreads);
SEXP rfsrcPartial(SEXP traceFlag,
                  SEXP seedPtr,
                  SEXP opt,
                  SEXP optHigh,
                  SEXP forestSize,
                  SEXP observationSize,
                  SEXP rSize,
                  SEXP rType,
                  SEXP rTarget,
                  SEXP rTargetCount,
                  SEXP rLevels,
                  SEXP rData,
                  SEXP xSize,
                  SEXP xType,
                  SEXP xLevels,
                  SEXP xData,
                  SEXP bootstrapSize,
                  SEXP caseWeight,
                  SEXP timeInterestSize,
                  SEXP timeInterest,
                  SEXP treeID,
                  SEXP nodeID,
                  SEXP parmID,
                  SEXP contPT,
                  SEXP mwcpSZ,
                  SEXP mwcpPT,
                  SEXP tnMCNT,
                  SEXP tnMEMB,
                  SEXP totalNodeCount,
                  SEXP seed,
                  SEXP partialType,
                  SEXP partialTarget,
                  SEXP partialLength,
                  SEXP partialValues,
                  SEXP numThreads);
char bootstrap (uint     mode,
                uint     treeID,
                Node    *nodePtr,
                uint    *subIndex,
                uint     subsetSize,
                uint    *index,
                uint     indexSize);
char getNodeSign (uint mode, uint treeID, Node *nodePtr, uint *bmIndex, uint repMembrSize);
void initializeTimeArrays(char mode);
void stackFactorArrays();
void stackFactorGeneric(uint    size,
                        char  **type,
                        uint  **p_factorMap,
                        uint   *factorCount,
                        uint  **p_factorIndex,
                        uint  **p_factorSize,
                        uint  **p_nonfactorMap,
                        uint   *nonfactorCount,
                        uint  **p_nonfactorIndex);
void unstackFactorArrays();
void initializeFactorArrays(char mode);
char stackMissingArrays(char mode);
void unstackMissingArrays(char mode);
void stackMissingSignatures(uint     obsSize,
                            uint     rspSize,
                            double **responsePtr,
                            double **predictorPtr,
                            uint    *recordMap,
                            uint     recordSize,
                            uint   **p_recordIndex,
                            uint    *p_vSize,
                            int   ***p_vSign,
                            int    **p_vIndex,
                            uint    *pRF_mrFactorSize,
                            uint   **pRF_mrFactorIndex,
                            uint    *pRF_mxFactorSize,
                            uint   **pRF_mxFactorIndex,
                            char    *pRF_mTimeFlag,
                            char    *pRF_mStatusFlag,
                            char    *pRF_mResponseFlag,
                            char    *pRF_mPredictorFlag);
void unstackMissingSignatures(uint      rspSize,
                              uint      recordSize,
                              uint     *recordIndex,
                              uint      vSize,
                              int     **vSign,
                              int      *vIndex,
                              uint      mrFactorSize,
                              uint     *mrFactorIndex,
                              uint      mxFactorSize,
                              uint     *mxFactorIndex);
char stackCompetingArrays(char mode);
void unstackCompetingArrays(char mode);
char stackClassificationArrays(char mode);
void unstackClassificationArrays(char mode);
void getEventTypeSize(uint     obsSize,
                      double  *status,
                      uint    *mRecordMap,
                      int    **mpSign,
                      uint    *eventTypeSize,
                      uint    *msize,
                      uint    *eventType);
void getClassLevelSize(uint      obsSize,
                       double  **response,
                       uint     *mRecordMap,
                       int     **mpSign,
                       uint     *classLevelSize,
                       uint    **classLevel);
void stackIncomingResponseArrays(uint mode);
void unstackIncomingResponseArrays(uint mode);
void unstackIncomingCovariateArrays(uint mode);
void unstackIncomingCovariateArrays(uint mode);
void stackIncomingArrays(uint mode);
void unstackIncomingArrays(uint mode);
void checkInteraction();
void stackPreDefinedCommonArrays();
void unstackPreDefinedCommonArrays();
void stackPreDefinedGrowthArrays();
void unstackPreDefinedGrowthArrays();
void stackPreDefinedRestoreArrays();
void unstackPreDefinedRestoreArrays();
void stackPreDefinedPredictArrays();
void unstackPreDefinedPredictArrays();
void stackWeights(double *weight,
                  uint    size,
                  uint   *weightType,
                  uint  **weightSorted,
                  uint   *weightDensitySize);
void unstackWeights(uint    weightType,
                    uint    size,
                    uint   *weightSorted);
extern void registerCustomFunctions();
uint stackDefinedOutputObjects(char      mode,
                               char    **sexpString,
                               Node   ***pRF_root,
                               uint    **pRF_tLeafCount,
                               double  **pRF_proximity,
                               int     **pRF_seed,
                               double  **p_imputation,
                               double ***pRF_sImputeResponsePtr,
                               double ***pRF_sImputePredictorPtr,
                               uint    **pRF_varUsed,
                               uint   ***pRF_varUsedPtr,
                               double  **p_splitDepth,
                               uint    **pRF_tTermMembershipIndex,
                               uint    **pRF_pNodeMembershipIndex,
                               uint    **pRF_bootstrapMembership,
                               uint     *stackCount,
                               SEXP     *sexpVector);
void unstackDefinedOutputObjects(char      mode,
                                 Node    **root);
uint stackVariableOutputObjects(char     mode,
                                uint   **p_treeID,
                                uint   **pRF_nodeID,
                                uint   **pRF_parmID,
                                double **pRF_contPT,
                                uint   **pRF_mwcpSZ,
                                uint   **pRF_mwcpPT,
                                uint   **pRF_mwcpCT,
                                uint     sexpLength,
                                char   **sexpString,
                                SEXP    *sexpVector);
uint stackStatisticalOutputObjects(char     mode,
                                   double **pRF_spltST,
                                   double **pRF_spltVR,
                                   uint   **pRF_uspvST,
                                   uint   **pRF_mtryID,
                                   double **pRF_mtryST,
                                   uint     sexpLength,
                                   char   **sexpString,
                                   SEXP    *sexpVector);
uint stackVariableTerminalNodeOutputObjects(char     mode,
                                            double **pRF_TN_SURV,
                                            double **pRF_TN_MORT,
                                            double **pRF_TN_NLSN,
                                            double **pRF_TN_CSHZ,
                                            double **pRF_TN_CIFN,
                                            double **pRF_TN_REGR,
                                            uint   **pRF_TN_CLAS,
                                            uint   **pRF_TN_MCNT,
                                            uint     sexpIndex,
                                            char   **sexpString,
                                            SEXP    *sexpVector);
void stackAuxVariableTerminalNodeOutputObjects(char    mode,
                                               double *pRF_TN_SURV,
                                               double *pRF_TN_MORT,
                                               double *pRF_TN_NLSN,
                                               double *pRF_TN_CSHZ,
                                               double *pRF_TN_CIFN,
                                               double *pRF_TN_REGR,
                                               uint   *pRF_TN_CLAS,
                                               uint   *pRF_TN_MCNT,
                                               uint   *pRF_TN_MEMB);
void unstackAuxVariableTerminalNodeOutputObjects(char mode);
void saveTerminalNodeOutputObjects (uint treeID);
void unstackAuxStatisticalOutputObjects(char mode);
void *stackAndProtect(uint  *sexpIndex,
                      char   sexpType,
                      uint   sexpIdentity,
                      uint   size,
                      SEXP  *sexpVector,
                      char **sexpString);
char getBestSplit(uint    treeID,
                  Node   *parent,
                  uint   *repMembrIndx,
                  uint    repMembrSize,
                  uint   *allMembrIndx,
                  uint    allMembrSize,
                  uint   *splitParameterMax,
                  double *splitValueMaxCont,
                  uint   *splitValueMaxFactSize,
                  uint  **splitValueMaxFactPtr,
                  double *splitStatistic,
                  char  **splitIndicator,
                  char    multImpFlag);
char randomSplit(uint    treeID,
                 Node   *parent,
                 uint   *repMembrIndx,
                 uint    repMembrSize,
                 uint   *allMembrIndx,
                 uint    allMembrSize,
                 uint   *splitParameterMax,
                 double *splitValueMaxCont,
                 uint   *splitValueMaxFactSize,
                 uint  **splitValueMaxFactPtr,
                 double *splitStatistic,
                 char  **splitIndicator,
                 char    multImpFlag);
typedef double (*customFunction) (uint    n,
                                  char   *membership,
                                  double *time,
                                  double *event,
                                  uint    eventTypeSize,
                                  uint    eventTimeSize,
                                  double *eventTime,
                                  double *response,
                                  double  mean,
                                  double  variance,
                                  uint    maxLevel);
void regCustomFunctionClassification (void *func, uint i);
void regCustomFunctionRegression (void *func, uint i);
void regCustomFunctionSurvival (void *func, uint i);
void regCustomFunctionCompetingRisk (void *func, uint i);
char classificationXwghtSplit(uint    treeID,
                              Node   *parent,
                              uint   *repMembrIndx,
                              uint    repMembrSize,
                              uint   *allMembrIndx,
                              uint    allMembrSize,
                              uint   *splitParameterMax,
                              double *splitValueMaxCont,
                              uint   *splitValueMaxFactSize,
                              uint  **splitValueMaxFactPtr,
                              double *splitStatistic,
                              char  **splitIndicator,
                              char    multImpFlag);
char regressionXwghtSplit(uint    treeID,
                          Node   *parent,
                          uint   *repMembrIndx,
                          uint    repMembrSize,
                          uint   *allMembrIndx,
                          uint    allMembrSize,
                          uint   *splitParameterMax,
                          double *splitValueMaxCont,
                          uint   *splitValueMaxFactSize,
                          uint  **splitValueMaxFactPtr,
                          double *splitStatistic,
                          char  **splitIndicator,
                          char    multImpFlag);
char logRankNCR(uint    treeID,
                Node   *parent,
                uint   *repMembrIndx,
                uint    repMembrSize,
                uint   *allMembrIndx,
                uint    allMembrSize,
                uint   *splitParameterMax,
                double *splitValueMaxCont,
                uint   *splitValueMaxFactSize,
                uint  **splitValueMaxFactPtr,
                double *splitStatistic,
                char  **splitIndicator,
                char    multImpFlag);
char logRankCR (uint    treeID,
                Node   *parent,
                uint   *repMembrIndx,
                uint    repMembrSize,
                uint   *allMembrIndx,
                uint    allMembrSize,
                uint   *splitParameterMax,
                double *splitValueMaxCont,
                uint   *splitValueMaxFactSize,
                uint  **splitValueMaxFactPtr,
                double *splitStatistic,
                char  **splitIndicator,
                char    multImpFlag);
void getMemberCountOnly(uint treeID);
char unsupervisedSplit(uint    treeID,
                       Node   *parent,
                       uint   *repMembrIndx,
                       uint    repMembrSize,
                       uint   *allMembrIndx,
                       uint    allMembrSize,
                       uint   *splitParameterMax,
                       double *splitValueMaxCont,
                       uint   *splitValueMaxFactSize,
                       uint  **splitValueMaxFactPtr,
                       double *splitStatistic,
                       char  **splitIndicator,
                       char    multImpFlag);
char multivariateSplit (uint    treeID,
                        Node   *parent,
                        uint   *repMembrIndx,
                        uint    repMembrSize,
                        uint   *allMembrIndx,
                        uint    allMembrSize,
                        uint   *splitParameterMax,
                        double *splitValueMaxCont,
                        uint   *splitValueMaxFactSize,
                        uint  **splitValueMaxFactPtr,
                        double *splitStatistic,
                        char  **splitIndicator,
                        char    multImpFlag);
char customMultivariateSplit (uint    treeID,
                              Node   *parent,
                              uint   *repMembrIndx,
                              uint    repMembrSize,
                              uint   *allMembrIndx,
                              uint    allMembrSize,
                              uint   *splitParameterMax,
                              double *splitValueMaxCont,
                              uint   *splitValueMaxFactSize,
                              uint  **splitValueMaxFactPtr,
                              double *splitStatistic,
                              char  **splitIndicator,
                              char    multImpFlag);
char customSurvivalSplit (uint    treeID,
                         Node   *parent,
                         uint   *repMembrIndx,
                         uint    repMembrSize,
                         uint   *allMembrIndx,
                         uint    allMembrSize,
                         uint   *splitParameterMax,
                         double *splitValueMaxCont,
                         uint   *splitValueMaxFactSize,
                         uint  **splitValueMaxFactPtr,
                         double *splitStatistic,
                         char  **splitIndicator,
                         char    multImpFlag);
char customCompetingRiskSplit (uint    treeID,
                               Node   *parent,
                               uint   *repMembrIndx,
                               uint    repMembrSize,
                               uint   *allMembrIndx,
                               uint    allMembrSize,
                               uint   *splitParameterMax,
                               double *splitValueMaxCont,
                               uint   *splitValueMaxFactSize,
                               uint  **splitValueMaxFactPtr,
                               double *splitStatistic,
                               char  **splitIndicator,
                               char    multImpFlag);
void stackSplitIndicator(uint   nodeSize,
                         char **localSplitIndicator);
void unstackSplitIndicator(uint  nodeSize,
                           char *localSplitIndicator);
void stackSplitEventTime(uint **localEventTimeCount,
                         uint **localEventTimeIndex);
void unstackSplitEventTime(uint *localEventTimeCount,
                           uint *localEventTimeIndex);
uint getSplitEventTime(uint   treeID,
                       uint   *repMembrIndx,
                       uint    repMembrSize,
                       uint   *nonMissMembrIndx,
                       uint    nonMissMembrSize,
                       uint   *localEventTimeCount,
                       uint   *localEventTimeIndex);
void stackSplitEventAndRisk(uint   eventTimeSize,
                            uint **nodeParentEvent,
                            uint **nodeParentAtRisk,
                            uint **nodeLeftEvent,
                            uint **nodeLeftAtRisk,
                            uint **nodeRightEvent,
                            uint **nodeRightAtRisk);
void unstackSplitEventAndRisk(uint  eventTimeSize,
                              uint *nodeParentEvent,
                              uint *nodeParentAtRisk,
                              uint *nodeLeftEvent,
                              uint *nodeLeftAtRisk,
                              uint *nodeRightEvent,
                              uint *nodeRightAtRisk);
void getSplitEventAndRisk(uint    treeID,
                          uint   *repMembrIndx,
                          uint    repMembrSize,
                          uint   *nonMissMembrIndx,
                          uint    nonMissMembrSize,
                          uint   *localEventTimeCount,
                          uint   *localEventTimeIndex,
                          uint    localEventTimeSize,
                          uint   *nodeParentEvent,
                          uint   *nodeParentAtRisk);
void stackAndGetSplitSurv(uint    treeID,
                          uint   *repMembrIndx,
                          uint    repMembrSize,
                          uint   *nonMissMembrIndx,
                          uint    nonMissMembrSize,
                          uint  **localEventTimeCount,
                          uint  **localEventTimeIndex,
                          uint   *localEventTimeSize,
                          uint  **nodeParentEvent,
                          uint  **nodeParentAtRisk,
                          uint  **nodeLeftEvent,
                          uint  **nodeLeftAtRisk,
                          uint  **nodeRightEvent,
                          uint  **nodeRightAtRisk);
void unstackSplitSurv(uint *localEventTimeCount,
                      uint *localEventTimeIndex,
                      uint  eventTimeSize,
                      uint *nodeParentEvent,
                      uint *nodeParentAtRisk,
                      uint *nodeLeftEvent,
                      uint *nodeLeftAtRisk,
                      uint *nodeRightEvent,
                      uint *nodeRightAtRisk);
uint stackAndConstructSplitVector(uint     treeID,
                                  uint     localMembershipSize,
                                  uint     randomCovariateIndex,
                                  double  *splitVector,
                                  uint     splitVectorSize,
                                  char    *factorFlag,
                                  char    *deterministicSplitFlag,
                                  uint    *mwcpSizeAbsolute,
                                  void   **splitVectorPtr);
void unstackSplitVector(uint   treeID,
                        uint   splitVectorSize,
                        uint   splitLength,
                        char   factorFlag,
                        char   deterministicSplitFlag,
                        uint   mwcpSizeAbsolute,
                        void  *splitVectorPtr);
void stackRandomCovariates(uint      treeID,
                           Node     *parent,
                           uint      repMembrSize,
                           char      multImpFlag,
                           uint    **covariateIndex,
                           uint     *covariateSize,
                           double  **cdf,
                           uint     *cdfSize,
                           uint    **cdfSort,
                           uint    **density,
                           uint     *densitySize,
                           uint   ***densitySwap);
void unstackRandomCovariates(uint     treeID,
                             Node     *parent,
                             uint    *covariateIndex,
                             uint     covariateSize,
                             double  *cdf,
                             uint     cdfSize,
                             uint    *cdfSort,
                             uint    *density,
                             uint     densitySize,
                             uint   **densitySwap,
                             uint     repMembrSize);
char selectRandomCovariates(uint     treeID,
                            Node     *parent,
                            uint     *repMembrIndx,
                            uint      repMembrSize,
                            uint     *covariateIndex,
                            uint     *uniformCovariateSize,
                            uint     *uniformCovariateIndex,
                            double   *cdf,
                            uint     *cdfSize,
                            uint     *cdfSort,
                            uint     *density,
                            uint     *densitySize,
                            uint    **densitySwap,
                            uint     *covariate,
                            uint     *actualCovariateCount,
                            uint     *candidateCovariateCount,
                            double   *splitVector,
                            uint     *splitVectorSize,
                            uint    **indxx,
                            uint      nonMissMembrSizeStatic,
                            uint     *nonMissMembrIndxStatic,
                            uint     *nonMissMembrSize,
                            uint    **nonMissMembrIndx,
                            char      multImpFlag);
void unselectRandomCovariates(uint      treeID,
                              Node     *parent,
                              uint      repMembrSize,
                              uint     *indxx,
                              uint      nonMissMembrSizeStatic,
                              uint     *nonMissMembrIndx,
                              char      multImpFlag);
uint virtuallySplitNode(uint  treeID,
                           char  factorFlag,
                           uint  mwcpSizeAbsolute,
                           uint  randomCovariate,
                           uint *repMembrIndx,
                           uint  repMembrSize,
                           uint *nonMissMembrIndx,
                           uint  nonMissMembrSize,
                           uint *indxx,
                           void *splitVectorPtr,
                           uint  offset,
                           char *localSplitIndicator,
                           uint *leftSize,
                           uint  priorMembrIter,
                           uint *currentMembrIter);
void getReweightedRandomPair(uint    treeID,
                             uint    relativefactorSize,
                             uint    absoluteFactorSize,
                             double *absoluteLevel,
                             uint   *result);
void getRandomPair(uint treeID, uint relativeFactorSize, uint absoluteFactorSize, double *absoluteLevel, uint *result);
void createRandomBinaryPair(uint    treeID,
                            uint    relativeFactorSize,
                            uint    absoluteFactorSize,
                            uint    groupSize,
                            double *absolutelevel,
                            uint   *pair);
void convertRelToAbsBinaryPair(uint    treeID,
                               uint    relativeFactorSize,
                               uint    absoluteFactorSize,
                               uint    relativePair,
                               double *absoluteLevel,
                               uint   *pair);
char summarizeSplitResult(uint    splitParameterMax,
                          double  splitValueMaxCont,
                          uint    splitValueMaxFactSize,
                          uint   *splitValueMaxFactPtr,
                          double *splitStatistic,
                          double  deltaMax);
char getPreSplitResult (uint      treeID,
                        Node     *parent,
                        uint      repMembrSize,
                        uint     *repMembrIndx,
                        uint     *nonMissMembrSize,
                        uint    **nonMissMembrIndx,
                        double  **splitVector,
                        double   *preSplitMean,
                        char      multImpFlag,
                        char      multVarFlag);
void unstackPreSplit (char      preliminaryResult,
                      char      multImpFlag,
                      char      multVarFlag,
                      uint      repMembrSize,
                      double   *splitVector,
                      uint     *nonMissMembrIndx);
char updateMaximumSplit(uint    treeID,
                        Node   *parent,
                        double  delta,
                        uint    candidateCovariateCount,
                        uint    covariate,
                        uint    index,
                        char    factorFlag,
                        uint    mwcpSizeAbsolute,
                        uint    repMembrSize,
                        char   *localSplitIndicator,
                        double *deltaMax,
                        uint   *splitParameterMax,
                        double *splitValueMaxCont,
                        uint   *splitValueMaxFactSize,
                        uint  **splitValueMaxFactPtr,
                        void   *splitVectorPtr,
                        char  **splitIndicator);
void updateNodeStatistics(Node *parent, double delta, uint candidateCovariateCount, uint covariate);
void getMeanResponseNew(uint       treeID,
                        Terminal  *parent,
                        uint      *repMembrIndx,
                        uint       repMembrSize,
                        uint      *allMembrIndx,
                        uint       allMembrSize);
void restoreMeanResponseNew(uint       treeID,
                            Terminal  *parent);
void updateEnsembleMean(uint     mode,
                        uint     treeID,
                        uint     serialTreeID,
                        char     omitDenominator);
double getMeanSquareError(uint    size,
                          double *responsePtr,
                          double *predictedOutcome,
                          uint   *oobCount);
char getVariance(uint    repMembrSize,
                 uint   *repMembrIndx,
                 uint    nonMissMembrSize,
                 uint   *nonMissMembrIndx,
                 double *targetResponse,
                 double *mean,
                 double *variance);
void restoreMeanResponse(uint treeID);
void getMultiClassProbNew (uint       treeID,
                           Terminal  *parent,
                           uint      *repMembrIndx,
                           uint       repMembrSize,
                           uint      *allMembrIndx,
                           uint       allMembrSize);
void restoreMultiClassProbNew (uint       treeID,
                               Terminal  *parent);
void updateEnsembleMultiClass(uint     mode,
                              uint     treeID,
                              uint     serialTreeID,
                              char     omitDenominator);
double getBrierScore(uint     obsSize,
                     uint     rTarget,
                     double  *responsePtr,
                     double **ensemblePtr,
                     uint    *denomCount,
                     double  *cpv);
void getConditionalClassificationIndex(uint     size,
                                       uint     rTargetFactor,
                                       double  *responsePtr,
                                       double  *outcomeCLS,
                                       uint    *denomCount,
                                       double  *cpv);
double getClassificationIndex(uint    size,
                              double *responsePtr,
                              double *predictedOutcome,
                              uint   *oobCount);
void restoreMultiClassProb(uint treeID);
void getAtRiskAndEventCountsNew (uint       treeID,
                                 Terminal  *parent,
                                 uint      *repMembrIndx,
                                 uint       repMembrSize,
                                 uint      *allMembrIndx,
                                 uint       allMembrSize);
void getLocalRatios (uint treeID, Terminal *parent);
void getLocalCSH (uint treeID, Terminal *parent);
void getLocalCIF (uint treeID, Terminal *parent);
void getLocalSurvival (uint treeID, Terminal *parent);
void getLocalNelsonAalen (uint treeID, Terminal *parent);
void getSurvival (uint treeID, Terminal *parent);
void getMortality (uint treeID, Terminal *parent);
void getNelsonAalen (uint treeID, Terminal *parent);
void getCSH (uint treeID, Terminal *parent);
void getCIF (uint treeID, Terminal *parent);
void restoreSurvival(uint treeID, Terminal *parent);
void restoreMortality(uint treeID, Terminal *parent);
void restoreNelsonAalen(uint treeID, Terminal *parent);
void restoreCSH(uint treeID, Terminal *parent);
void restoreCIF(uint treeID, Terminal *parent);
void updateEnsembleSurvival(uint mode,
                            uint treeID,
                            uint serialTreeID);
void getEnsembleMortality(uint      mode, 
                          uint      treeID,
                          uint      obsSize,
                          double  **ensembleMRTptr,
                          uint     *ensembleDen,
                          double   *mortality);
void getEnsembleMortalityCR(uint      mode, 
                            uint      treeID,
                            uint      obsSize,
                            double  **ensembleMRTptr,
                            uint     *ensembleDen,
                            double  **cMortality);
void getConditionalConcordanceArrays(uint     j, 
                                     double  *timePtr, 
                                     double  *statusPtr, 
                                     double  *mortalityPtr, 
                                     uint    *genericEnsembleDenPtr,
                                     uint    *meIndividualSize,
                                     uint   **eIndividual,
                                     double  *subsettedTime,
                                     double  *subsettedStatus,
                                     double  *subsettedMortality,
                                     uint    *subsettedEnsembleDen);
double getConcordanceIndex(int     polarity,
                           uint    size, 
                           double *timePtr, 
                           double *statusPtr, 
                           double *predictedOutcome,
                           uint   *oobCount);
void getCRPerformance (uint     mode,
                       uint     obsSize,
                       double **responsePtr,
                       double **yearsLost,
                       uint    *denom,
                       double  *performanceVector);
char imputeNode (uint     type,
                 char     termFlag,
                 char     chainFlag,
                 uint     treeID, 
                 Node    *nodePtr,
                 uint    *repAbsIdx,
                 uint     repNodeSize,
                 uint    *iAbsIdx,
                 uint     iNodeSize);
char restoreNodeMembership(uint  r,
                           uint  mode, 
                           char  rootFlag,
                           uint  treeID, 
                           Node *parent, 
                           uint *repMembrIndx,
                           uint  repMembrSize,
                           uint *allMembrIndx,
                           uint  allMembrSize,
                           uint *ngAllMembrIndx,
                           uint  ngAllMembrSize,
                           uint *bootMembrIndxIter);
void imputeUpdateShadow (uint      mode, 
                         double  **shadowResponse, 
                         double  **shadowPredictor);
void imputeUpdateSummary (uint     mode, 
                          uint     treeID);
void imputeNodeAndSummarize(uint     r,
                            uint     type,
                            uint     treeID,
                            Node    *parent,
                            uint    *repMembrIndx,
                            uint     repMembrSize,
                            uint    *allMembrIndx,
                            uint     allMembrSize,
                            uint    *ngAllMembrIndx,
                            uint     ngAllMembrSize);
void imputeSummary(uint      mode,
                   char      selectionFlag);
void imputeResponse(uint      mode,
                       uint      treeID,
                       double  **tempResponse);
void imputeCommon(uint      mode,
                  uint      treeID,
                  char      selectionFlag,
                  char      predictorFlag);
void imputeMultipleTime (char selectionFlag);
double getNearestMasterTime (double   meanvalue,
                             char     chainFlag,
                             uint     treeID);
double getMaximalValue(double *value, uint size, char chainFlag, uint treeID);
double getMedianValue(double *value, uint size);
double getMeanValue(double *value, uint size);
double getSampleValue(double *value, uint size, char chainFlag, uint treeID);
uint getRecordMap(uint     *map, 
                  uint      size, 
                  double  **resp, 
                  double  **data);
void updateTimeIndexArray(uint    treeID,
                          uint   *allMemberIndx,
                          uint    allMembrSize,
                          double *time, 
                          char    naflag,
                          char    idFlag,
                          uint   *masterTimeIndex);
void updateEventTypeSubsets(double *summaryStatus, 
                            uint    mRecordSize,
                            int   **mpSign,
                            uint   *mRecordIndex,
                            uint   *meIndividualSize,
                            uint  **eIndividual);
void stackShadow (uint mode, uint treeID);
void unstackShadow (uint mode, uint treeID, char respFlag, char covrFlag);
char xferMissingness(uint type, Node *source, Terminal *destination);
char getMarginalNodeMembership(uint     mode,
                               char     rootFlag,
                               uint     treeID,
                               Node    *parent,
                               uint    *allMembrIndx,
                               uint     allMembrSize,
                               double **observationPtr);
void acquireTree(uint mode, uint r, uint b);
void finalizeProximity(uint mode);
void updateProximity(uint mode, uint b);
void updateSplitDepth(uint treeID, Node *rootPtr, uint maxDepth);
char pruneBranch(uint obsSize, uint treeID, Node **nodesAtDepth, uint nadCount, uint ptnTarget, uint ptnCurrent);
uint pruneTree(uint obsSize, uint treeID, uint ptnCount);
void unstackAuxiliary2(uint mode, uint b);
void unstackAuxiliary(uint mode, uint b);
void stackNodeAndTermList(uint treeID, uint length);
void unstackNodeList(uint treeID);
void unstackTermList(uint treeID);
void stackAndInitTermMembership(uint mode, uint treeID);
void unstackTermMembership(uint mode, uint treeID);
Node *getTerminalNode(uint treeID, uint leaf);
void getRawNodeSize(uint  type,
                    uint  treeID,
                    Node *parent,
                    uint *repMembrIndx,
                    uint *repMembrSize,
                    uint *allMembrIndx,
                    uint *allMembrSize);
char forkAndUpdate(uint    treeID,
                   Node   *parent,
                   uint   *repMembrIndx,
                   uint    repMembrSize,
                   uint   *allMembrIndx,
                   uint    allMembrSize,
                   uint    splitParameterMax,
                   double  splitValueMaxCont,
                   uint    splitValueMaxFactSize,
                   uint   *splitValueMaxFactPtr,
                   double  splitStatistic,
                   char   *localSplitIndicator,
                   char    multImpFlag,
                   char   *membershipIndicator,
                   uint   *leftDaughterSize,
                   uint   *rightDaughterSize);
char growTree(uint     r,
              char     rootFlag,
              char     multImpFlag,
              uint     b,
              Node    *parent,
              uint    *repMembrIndx,
              uint     repMembrSize,
              uint    *allMembrIndx,
              uint     allMembrSize,
              uint     depth,
              uint    *maximumDepth,
              uint    *bootMembrIndxIter);
char restoreTree(uint    mode,
                 uint    b,
                 Node   *parent,
                 uint   *offset,
                 uint   *treeID,
                 uint   *nodeID,
                 uint   *parmID,
                 double *contPT,
                 uint   *mwcpSZ,
                 uint  **mwcpPtr,
                 uint    depth,
                 uint   *maximumDepth);
void saveTree(uint    b,
              Node   *parent,
              uint   *offset,
              uint   *treeID,
              uint   *nodeID,
              uint   *parmID,
              double *contPT,
              uint   *mwcpSZ,
              uint  **mwcpPtr,
              uint   *mwcpCT);
void freeTree(uint treeID, Node *parent, char rootFlag);
void getSplitDepth(Node *parent, uint *maximumDepth);
void freeSplitDepth(uint treeID);
void saveStatistics(char     mode,
                    uint     b,
                    Node    *parent,
                    uint    *offset,
                    double  *spltST,
                    double  *spltVR,
                    uint   **uspvST,
                    uint   **mtryID,
                    double **mtryST);
uint getMaximumDepth(Node *parent);
void getNodesAtDepth(Node *parent, uint tagDepth, Node **nodesAtDepth, uint *nadCount);
void getTreeInfo(uint treeID, Node *parent);
void getPTNodeList(Node    *parent,
                   Node   **list,
                   uint    *offset);
void restoreNodeMembershipGrow(uint treeID);
char getRestoreNodeMembershipGrowFlag(char mode);
void updateTerminalNodeOutcomesNew (uint       treeID,
                                    Terminal  *parent,
                                    uint      *repMembrIndx,
                                    uint       repMembrSize,
                                    uint      *allMembrIndx,
                                    uint       allMembrSize);
void restoreTerminalNodeOutcomes (uint       treeID,
                                  Terminal  *parent);
void restoreMembrCount(uint treeID);
void updateEnsembleCalculations (char      multipleImputeFlag,
                                 uint      mode,
                                 uint      b);
char stackAndImputePerfResponse(uint      mode,
                                char      multipleImputeFlag,
                                uint      treeID,
                                uint      serialID,
                                double ***responsePtr);
double **stackAndImputeGenericResponse(char flag, uint mode, uint obsSize, uint treeID, uint serialID, double **responsePtr);
void unstackImputeResponse(char flag, uint obsSize, double **mResponsePtr);
void getPerformance(uint      serialTreeID,
                    uint      mode,
                    uint      obsSize,
                    double  **responsePtr,
                    uint      *denomPtr,
                    double   **outcomeMRT,
                    double  ***outcomeCLS,
                    double   **outcomeRGR,
                    double   *perfMRTptr,
                    double  **perfCLSptr,
                    double   *perfRGRptr);
void finalizeEnsembleEstimates(uint mode);
char getPerformanceFlag (uint mode, uint serialTreeID);
void getVariablesUsed(uint treeID, Node *rootPtr, uint *varUsedVector);
Node *identifyPerturbedMembership(Node    *parent,
                                  double **shadowVIMP,
                                  uint     index);
Node *randomizeMembership(Node    *parent,
                          double **predictor,
                          uint     individual,
                          uint     splitParameter,
                          uint     treeID);
Node *antiMembership(Node    *parent,
                     double **predictor,
                     uint     individual,
                     uint     splitParameter,
                     uint     treeID);
void permute(uint ranGenID, uint p, uint n, uint *indx);
void getAntiMembership(uint       mode,
                       uint       treeID,
                       Terminal **vimpMembership,
                       uint       p);
void getRandomMembership(uint       mode,
                         uint       treeID,
                         Terminal **vimpMembership,
                         uint       p);
void getPermuteMembership(uint       mode,
                          uint       treeID,
                          Terminal **vimpMembership,
                          uint       p);
void getVimpMembership(uint      mode,
                       uint      treeID,
                       Terminal    **vimpMembership,
                       uint      p);
void updateGenericVimpEnsemble (uint       mode,
                                uint       treeID,
                                uint       xVarIdx,
                                Terminal **noiseMembership,
                                char       ensembleFlag,
                                double  ***genEnsembleMRT,
                                double ****genEnsembleCLS,
                                double  ***genEnsembleRGR);
void updateTreeEnsemble (uint       mode,
                         uint       treeID,
                         double  ***treeEnsembleMRT,
                         double ****treeEnsembleCLS,
                         double  ***treeEnsembleRGR);
void updateVimpEnsemble (uint       mode,
                         uint       treeID,
                         Terminal **vimpMembership,
                         uint       p);
void summarizeVimpPerformance(uint       mode,
                              uint       treeID,
                              uint       p);
void finalizeVimpPerformance(uint mode, uint rejectedTreeCount);
void  stackVimpMembership(uint mode, Terminal ***membership);
void  unstackVimpMembership(uint mode, Terminal **membership);
void stackTreeEnsemble(uint         mode,
                       uint         treeID,
                       uint       **denomTree,
                       double   ****treeOutcome,
                       double  *****sTreeOutcome,
                       double   ****mcTreeOutcome);
void unstackTreeEnsemble(uint       mode,
                         uint       treeID,
                         uint      *denomTree,
                         double  ***treeOutcome,
                         double ****sTreeOutcome,
                         double  ***mcTreeOutcome);
void updateVimpCalculations (uint mode, uint b, uint intrIndex, Terminal **vimpMembership);
void summarizeTreePerformance(uint mode, uint treeID);
SEXP rfsrc(char mode, int seedValue, uint traceFlag);
