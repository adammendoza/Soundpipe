/*
 * Del
 * 
 * This code has been extracted from the Csound opcode "vdelay3".
 * It has been modified to work as a Soundpipe module.
 * 
 * Original Author(s): Paris Smaradgis, Istvan Varga
 * Year: 1994
 * Location: OOps/vdelay.c
 *
 */

#include <stdlib.h>
#include "soundpipe.h"

int sp_del_create(sp_del **p) 
{
    *p = malloc(sizeof(sp_del));
    return SP_OK;
}

int sp_del_destroy(sp_del **p) 
{
    sp_del *pp = *p;
    sp_auxdata_free(&pp->buf);
    free(*p);
    return SP_OK;
}

int sp_del_init(sp_data *sp, sp_del *p, SPFLOAT maxdel) 
{
    uint32_t n = (int32_t)(maxdel * sp->sr)+1;
    p->sr = sp->sr;
    p->del = maxdel;
    p->maxdel = maxdel;
    sp_auxdata_alloc(&p->buf, n * sizeof(SPFLOAT));
    p->left = 0;
    return SP_OK;
}

int sp_del_compute(sp_data *sp, sp_del *p, SPFLOAT *in, SPFLOAT *out) 
{
    int32_t  maxd, indx;
    *out = p->sr;  
    SPFLOAT del = p->del;
    SPFLOAT b0, b1, b2, b3;
    int32_t v0, v1, v2, v3;
    SPFLOAT  fv1;
    indx = p->left;

    sp_auxdata_setbuf(&p->buf, indx, in);

    fv1 = del * (-1.0 * sp->sr);
    v1 = (int32_t)fv1;
    fv1 -= (SPFLOAT) v1;
    v1 += (int32_t)indx;
    maxd = (uint32_t) (p->maxdel * p->sr);
    /* Make sure we're inside the buffer */
    if ((v1 < 0) || (fv1 < 0.0)) {
        fv1++; v1--; 
        while (v1 < 0) { 
            v1 += (int32_t)maxd;
        }
    } else {
        while (v1 >= (int32_t)maxd) {
        v1 -= (int32_t)maxd;
        }
    }
    /* Find next sample for interpolation      */
    v2 = (v1 == (int32_t)(maxd - 1UL) ? 0L : v1 + 1L);

    if (maxd<4) {
        sp_auxdata_getbuf(&p->buf, v1, &b1);
        sp_auxdata_getbuf(&p->buf, v2, &b2);
        *out = b1 + fv1 * (b2 - b1);
    } else {
        v0 = (v1==0 ? maxd-1 : v1-1);
        v3 = (v2==(int32_t)maxd-1 ? 0 : v2+1);
        {                     
            SPFLOAT w, x, y, z;
            z = fv1 * fv1; z--; 
            z *= 0.1666666667;
            y = fv1; 
            y++; w = (y *= 0.5); w--;
            x = 3.0 * z; y -= x; w -= z; x -= fv1;
            sp_auxdata_getbuf(&p->buf, v0, &b0);
            sp_auxdata_getbuf(&p->buf, v1, &b1);
            sp_auxdata_getbuf(&p->buf, v2, &b2);
            sp_auxdata_getbuf(&p->buf, v3, &b3);
            *out = (w*b0 + x*b1 + y*b2 + z*b3)
            * fv1 + b1;
        }
    }
    if (++indx == maxd) indx = 0;             
    p->left = indx;             
    return SP_OK;
}
