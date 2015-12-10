#include <stdio.h>
#include <stdlib.h>

#include "vcl.h"
#include "vrt.h"
#include "cache/cache.h"

#include "vcc_if.h"

int
init_function(const struct vrt_ctx *ctx, struct vmod_priv *priv,
    enum vcl_event_e e)
{
	if (e != VCL_EVENT_LOAD)
		return (0);

	/* init what you need */
	return (0);
}

void dump_VSL_split(struct req *req, char *p, char *p2, unsigned mlen2, const void *ptr, size_t l, unsigned br){
	size_t length = l;
	void *c_ptr = (void*)ptr;
	txt t;
	t.b = p;
	
	do{
		if(mlen2 > length){
			memcpy(p2, c_ptr, length);
			t.e = t.b + length + 6;
			if(br){
				memcpy(p2 + length, "\r\n", 2);
				t.e += 2;
			}
			VSLbt(req->vsl, SLT_Debug,t);
			break;
		}
		memcpy(p2, c_ptr, mlen2);
		t.e = t.b + mlen2 + 6;
		if(br){
			memcpy(p2 + mlen2, "\r\n", 2);
			t.e += 2;
		}
		VSLbt(req->vsl, SLT_Debug,t);
		c_ptr  += mlen2;
		length -= mlen2;
		
	}while(1);
}
static int __match_proto__(req_body_iter_f)
vbf_printRequestBody(struct req *req, void *priv, void *ptr, size_t l)
{
    CHECK_OBJ_NOTNULL(req, REQ_MAGIC);
	unsigned mlen,mlen2;
	unsigned u;
	char *p,*p2;
	mlen = cache_param->vsl_reclen -3;
	u = WS_Reserve(req->ws, 0);
	if(u < mlen) {
		//no space.
		WS_Release(req->ws, 0);
		return (0);
	}
	p = req->ws->f;
	strcpy(p,"DUMP: ");
	
	p2 = p + 6;
	mlen2 = mlen - 6;
	dump_VSL_split(req,p,p2,mlen2,priv,strlen(priv),0);
	
	//header output
	for(int i=0; i < req->http0->nhd; ++i) {
		if (req->http0->hd[i].b == NULL && req->http0->hd[i].e == NULL)	continue;
		dump_VSL_split(req, p, p2, mlen2, req->http0->hd[i].b, (req->http0->hd[i].e - req->http0->hd[i].b), 1);
	}
	dump_VSL_split(req,p,p2,mlen2,"\r\n",2,0);
	
	//post body
	if (l > 0){
		dump_VSL_split(req, p, p2, mlen2, ptr, l, 0);
    }
	//free work-space
	WS_Release(req->ws, 0);
    return (0);
}
VCL_VOID
vmod_req(VRT_CTX, VCL_STRING val)
{
	VRB_Iterate(ctx->req, vbf_printRequestBody, (void*)val);
}
