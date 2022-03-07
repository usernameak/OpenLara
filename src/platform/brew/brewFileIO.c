#include "brewFileIO.h"

#include <AEEShell.h>
#include <errno.h>

static IFileMgr *pIFileMgr = NULL;

void bfio_init(IShell *pIShell) {
    ISHELL_CreateInstance(pIShell, AEECLSID_FILEMGR, (void**) &pIFileMgr);
}

void bfio_dispose() {
    IFILEMGR_Release(pIFileMgr);
}

static void bfio_seterrno() {
    switch(IFILEMGR_GetLastError(pIFileMgr)) {
        case SUCCESS:
        case EFILEEOF:
            errno = 0;
            break;
        case EBADSEEKPOS:
        case EBADFILENAME:
        case EBADPARM:
            errno = EINVAL;
            break;
        case EFILEEXISTS:
            errno = EEXIST;
            break;
        case EFILENOEXISTS:
            errno = ENOENT;
            break;
        case EDIRNOTEMPTY:
            errno = ENOTEMPTY;
            break;
        case EFSFULL:
            errno = ENOSPC;
            break;
        case EFILEOPEN:
            errno = EBUSY;
            break;
        default:
            errno = EIO;
            break;
    }
}

FILE *bfio_fopen(const char *fname, const char *mode) {
    int openFileMode = 0;
    char modeChar1 = mode[0];
    int doCreate = 0;
    int doTruncate = 0;
    switch(modeChar1) {
    case 'r':
        openFileMode = _OFM_READ;
        break;
    case 'w':
        openFileMode = _OFM_READWRITE;
        doCreate = 1;
        doTruncate = 1;
        break;
    case 'a':
        openFileMode = _OFM_APPEND;
        doCreate = 1;
        break;
    default:
    case '\0':
        errno = EINVAL;
        return NULL;
    }
    char modeChar2 = mode[1];
    if(modeChar1 == 'r' && modeChar2 == '+') {
        openFileMode = _OFM_READWRITE;
    }
    FILE *fp = IFILEMGR_OpenFile(pIFileMgr, fname, openFileMode);
    bfio_seterrno();

    if(EFILENOEXISTS == IFILEMGR_GetLastError(pIFileMgr)) {
        if(doCreate) {
            FILE *fp = IFILEMGR_OpenFile(pIFileMgr, fname, _OFM_CREATE);
            bfio_seterrno();
        }
    } else if(doTruncate) {
        IFILE_Truncate(fp, 0);
    }

    return fp;
}

size_t bfio_fwrite(const void *buf, size_t size, size_t count, FILE *stream) {
    size_t numWritten = IFILE_Write(stream, buf, size * count) / size;
    bfio_seterrno();
    return numWritten;
}

size_t bfio_fread(void *buf, size_t size, size_t count, FILE *stream) {
    size_t numRead = IFILE_Read(stream, buf, size * count) / size;
    bfio_seterrno();
    return numRead;
}

long bfio_ftell(FILE *stream) {
    return IFILE_Seek(stream, _SEEK_CURRENT, 0);
}

int bfio_fseek (FILE *stream, long offset, int origin) {
    if(offset == 0 && origin == SEEK_CUR) return 0;
    FileSeekType st;
    switch(origin) {
    case SEEK_SET:
        st = _SEEK_START;
        break;
    case SEEK_CUR:
        st = _SEEK_CURRENT;
        break;
    case SEEK_END:
        st = _SEEK_END;
        break;
    }
    int res = IFILE_Seek(stream, st, offset);
    bfio_seterrno();
    if(res == SUCCESS) {
        return 0;
    } else {
        return -1;
    }
}

int bfio_fclose(FILE *fp) {
    IFILE_Release(fp);
    return 0;
}
