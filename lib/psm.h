#ifndef H_PSM
#define H_PSM

/** \file lib/psm.h
 */

#include <rpmlib.h>
#include "depends.h"
#include "fsm.h"

/**
 */
struct sharedFile {
    int mainFileNumber;
    int secRecOffset;
    int secFileNumber;
} ;

/**
 */
struct sharedFileInfo {
    int pkgFileNum;
    int otherFileNum;
    int otherPkg;
    int isRemoved;
};

/**
 */
struct transactionFileInfo_s {
  /* for all packages */
    enum rpmTransactionType type;
    fileAction action;		/*!< File disposition default. */
/*@owned@*/ fileAction * actions;	/*!< File disposition(s) */
/*@owned@*/ struct fingerPrint_s * fps;	/*!< File fingerprint(s) */
    HGE_t hge;			/*!< Vector to headerGetEntry() */
    HFD_t hfd;			/*!< Vector to headerFreeData() */
    Header h;			/*!< Package header */
/*@owned@*/ const char * name;
/*@owned@*/ const char * version;
/*@owned@*/ const char * release;
    int_32 epoch;
    uint_32 flags;		/*!< File flag default. */
    const uint_32 * fflags;	/*!< File flag(s) (from header) */
    const uint_32 * fsizes;	/*!< File size(s) (from header) */
    const uint_32 * fmtimes;	/*!< File modification time(s) (from header) */
/*@owned@*/ const char ** bnl;	/*!< Base name(s) (from header) */
/*@owned@*/ const char ** dnl;	/*!< Directory name(s) (from header) */
    int_32 * dil;		/*!< Directory indice(s) (from header) */
/*@owned@*/ const char ** obnl;	/*!< Original base name(s) (from header) */
/*@owned@*/ const char ** odnl;	/*!< Original directory name(s) (from header) */
    int_32 * odil;	/*!< Original directory indice(s) (from header) */
/*@owned@*/ const char ** fmd5s;/*!< File MD5 sum(s) (from header) */
/*@owned@*/ const char ** flinks;	/*!< File link(s) (from header) */
/* XXX setuid/setgid bits are turned off if fuser/fgroup doesn't map. */
    uint_16 * fmodes;		/*!< File mode(s) (from header) */
/*@owned@*/ char * fstates;	/*!< File state(s) (from header) */
/*@owned@*/ const char ** fuser;	/*!< File owner(s) */
/*@owned@*/ const char ** fgroup;	/*!< File group(s) */
/*@owned@*/ const char ** flangs;	/*!< File lang(s) */
    int fc;			/*!< No. of files. */
    int dc;			/*!< No. of directories. */
    int bnlmax;			/*!< Length (in bytes) of longest base name. */
    int dnlmax;			/*!< Length (in bytes) of longest dir name. */
    int astriplen;
    int striplen;
    unsigned int archiveSize;
    mode_t dperms;		/*!< Directory perms (0755) if not mapped. */
    mode_t fperms;		/*!< File perms (0644) if not mapped. */
/*@owned@*/ const char ** apath;
    int mapflags;
/*@owned@*/ int * fmapflags;
    uid_t uid;
/*@owned@*/ /*@null@*/ uid_t * fuids;	/*!< File uid(s) */
    gid_t gid;
/*@owned@*/ /*@null@*/ gid_t * fgids;	/*!< File gid(s) */
    int magic;
#define	TFIMAGIC	0x09697923
/*@owned@*/ FSM_t fsm;		/*!< File state machine data. */

  /* these are for TR_ADDED packages */
/*@dependent@*/ struct availablePackage * ap;
/*@owned@*/ struct sharedFileInfo * replaced;
/*@owned@*/ uint_32 * replacedSizes;
  /* for TR_REMOVED packages */
    unsigned int record;
};

/**
 */
#define	PSM_VERBOSE	0x8000
#define	PSM_INTERNAL	0x4000
#define	PSM_SYSCALL	0x2000
#define	PSM_DEAD	0x1000
#define	_fv(_a)		((_a) | PSM_VERBOSE)
#define	_fi(_a)		((_a) | PSM_INTERNAL)
#define	_fs(_a)		((_a) | (PSM_INTERNAL | PSM_SYSCALL))
#define	_fd(_a)		((_a) | (PSM_INTERNAL | PSM_DEAD))
typedef enum pkgStage_e {
    PSM_UNKNOWN =  0,
    PSM_INIT	=  1,
    PSM_PRE	=  2,
    PSM_PROCESS	=  3,
    PSM_POST	=  4,
    PSM_UNDO	=  5,
    PSM_FINI	=  6,
    PSM_NOTIFY	=  7,
    PSM_COMMIT	=  8,
    PSM_CREATE	=  9,
    PSM_DESTROY	=  10,
    PSM_CHROOT_IN= 11,
    PSM_CHROOT_OUT=12,
    PSM_SCRIPT	=  13,
    PSM_TRIGGER	=  14,
} pkgStage;
#undef	_fv
#undef	_fi
#undef	_fs
#undef	_fd

/**
 */
struct psm_s {
    rpmTransactionSet ts;
    TFI_t fi;
    int scriptTag;		/*!< Scriptlet data tag. */
    int progTag;		/*!< Scriptlet interpreter tag. */
    int scriptArg;		/*!< No. of installed instances. */
    int sense;			/*!< One of RPMSENSE_TRIGGER{IN,UN,POSTUN}. */
    int countCorrection;	/*!< 0 if installing, -1 if removing. */
    int chrootDone;		/*!< Was chroot(2) done by pkgStage? */
    int rc;
    pkgStage stage;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Load data from header into transaction file element info.
 * @param h		header
 * @param fi		transaction element file info
 */
void loadFi(Header h, TFI_t fi)
	/*@modifies h, fi @*/;

/**
 * Destroy transaction element file info.
 * @param fi		transaction element file info
 */
void freeFi(TFI_t fi)
	/*@modifies fi @*/;

/**
 * Return formatted string representation of package disposition.
 * @param a		package dispostion
 * @return		formatted string
 */
/*@observer@*/ const char *const fiTypeString(TFI_t fi);

/**
 * Return formatted string representation of file disposition.
 * @param a		file dispostion
 * @return		formatted string
 */
/*@observer@*/ const char *const fileActionString(fileAction a);

/**
 * Install binary package (from transaction set).
 * @param psm		package state machine data
 * @return		0 on success, 1 on bad magic, 2 on error
 */
int installBinaryPackage(PSM_t psm)
	/*@modifies psm @*/;

/**
 * Erase binary package (from transaction set).
 * @param psm		package state machine data
 * @return		0 on success
 */
int removeBinaryPackage(PSM_t psm)
	/*@modifies psm @*/;

/**
 * @param psm		package state machine data
 * @return		0 on success
 */
int repackage(PSM_t psm)
	/*@modifies psm @*/;

/**
 */
int psmStage(PSM_t psm, pkgStage stage)
	/*@modifies psm @*/;

#ifdef __cplusplus
}
#endif

#endif	/* H_ROLLBACK */
