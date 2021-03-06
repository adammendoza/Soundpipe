#include <string.h>
#include <stdlib.h>
#include "soundpipe.h"

int sp_reverse_create(sp_data *sp, sp_reverse **p)
{
    *p = malloc(sizeof(sp_reverse));
    return SP_OK;
}

int sp_reverse_destroy(sp_reverse **p)
{
    sp_reverse *pp = *p;
    sp_auxdata_free(&pp->buf);
    free(*p);
    return SP_OK;
}

int sp_reverse_init(sp_data *sp, sp_reverse *p, SPFLOAT delay)
{
    size_t size = delay * sp->sr * sizeof(SPFLOAT) * 2;
    p->bufpos = 0;
    sp_auxdata_alloc(&p->buf, size);
    p->bufsize = p->buf.size / sizeof(SPFLOAT);
    return SP_OK;
}

int sp_reverse_compute(sp_data *sp, sp_reverse *p, SPFLOAT *in, SPFLOAT *out)
{
    sp_auxdata_getbuf(&p->buf, p->bufpos, out);
    sp_auxdata_setbuf(&p->buf, (p->bufsize - 1) - p->bufpos, in);
    p->bufpos = (p->bufpos + 1) % p->bufsize;
    return SP_OK;
}
