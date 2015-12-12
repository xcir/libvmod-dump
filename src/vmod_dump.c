#include <stdio.h>
#include <stdlib.h>

#include "vcl.h"
#include "vrt.h"
#include "cache/cache.h"
#include "cache/cache_filter.h"

#include "vcc_if.h"

#define VMOD_DUMP_PRE      "VMD-DUMP"
#define VMOD_DUMP_KEY      "VMD-DUMP: "
#define VMOD_DUMP_KEY_LEN  10

int
init_function(const struct vrt_ctx *ctx, struct vmod_priv *priv,
    enum vcl_event_e e)
{
	if (e != VCL_EVENT_LOAD)
		return (0);

	/* init what you need */
	return (0);
}

void dump_VSL_split(struct req *req, char *p, unsigned mlen, const void *ptr, size_t l, unsigned br){
	
	char *p2       = p + VMOD_DUMP_KEY_LEN;
	unsigned mlen2 = mlen - VMOD_DUMP_KEY_LEN;
	size_t length  = l;
	void *c_ptr    = (void*)ptr;
	txt t;
	t.b = p;
	
	do{
		if(mlen2 > length){
			memcpy(p2, c_ptr, length);
			t.e = t.b + length + VMOD_DUMP_KEY_LEN;
			if(br) {
				memcpy(p2 + length, "\r\n", 2);
				t.e += 2;
			}
			VSLbt(req->vsl, SLT_Debug,t);
			break;
		}
		memcpy(p2, c_ptr, mlen2);
		t.e = t.b + mlen2 + VMOD_DUMP_KEY_LEN;
		if(br) {
			memcpy(p2 + mlen2, "\r\n", 2);
			t.e += 2;
		}
		VSLbt(req->vsl, SLT_Debug,t);
		c_ptr  += mlen2;
		length -= mlen2;
		
	}while(1);
}

int work_head(struct req *req, char *p,unsigned plen,  void *ptr, size_t l, struct http *http)
{
	CHECK_OBJ_NOTNULL(req, REQ_MAGIC);
	unsigned mlen;
	
	//get vsl max size per line.
	if(plen < cache_param->vsl_reclen) {
		mlen = plen  - 3;
	}else{
		mlen = cache_param->vsl_reclen  - 3;
	}
	//add prefix
	strcpy(p, VMOD_DUMP_KEY);
	
	//valここでおちてるのでNULLチェック
//		if(priv){
//			dump_VSL_split(req, p, mlen, priv, strlen(priv), 0);
//		}
	
	//header
	for(int i=0; i < http->nhd; ++i) {
		if (http->hd[i].b == NULL && http->hd[i].e == NULL)	continue;
		dump_VSL_split(req, p, mlen, http->hd[i].b, (http->hd[i].e - http->hd[i].b), 1);
	}
		dump_VSL_split(req,p,mlen,"\r\n",2,0);
    return (0);

}

int work_body(struct req *req, char *p,unsigned plen, void *ptr, size_t l, struct http *http)
{
	CHECK_OBJ_NOTNULL(req, REQ_MAGIC);
	unsigned mlen;
	
	//get vsl max size per line.
	if(plen < cache_param->vsl_reclen) {
		mlen = plen - VMOD_DUMP_KEY_LEN - 2;
	}else{
		mlen = cache_param->vsl_reclen - VMOD_DUMP_KEY_LEN - 2;
	}
	//add prefix
	strcpy(p, VMOD_DUMP_KEY);
	
	//body
	if (l > 0){
		dump_VSL_split(req, p,  mlen, ptr, l, 0);
    }
    return (0);

}

static int __match_proto__(req_body_iter_f)
vbf_printRequestBody(struct req *req, void *priv, void *ptr, size_t l)
{
	unsigned u;
	//reserve work-space
	u = WS_Reserve(req->ws, 0);
	if(u <= VMOD_DUMP_KEY_LEN) {
		//no space.
		WS_Release(req->ws, 0);
		return (0);		
	}
	work_head(req,req->ws->f, u, ptr, l, req->http0);
	work_body(req,req->ws->f, u, ptr, l, req->http0);
	
	//free work-space
	WS_Release(req->ws, 0);
	return(0);
}

VCL_VOID
vmod_req(VRT_CTX, VCL_STRING val)
{
	VSLb(ctx->req->vsl, SLT_Debug,"%s-S: REQ", VMOD_DUMP_PRE);
	VSLb(ctx->req->vsl, SLT_Debug,"%s-V: %s", VMOD_DUMP_PRE, val);
	VRB_Iterate(ctx->req, vbf_printRequestBody, NULL);
	VSLb(ctx->req->vsl, SLT_Debug,"%s-S: END", VMOD_DUMP_PRE);
}



int __match_proto__(vdp_bytes)
VDP_dump(struct req *req, enum vdp_action act, void **priv,
    const void *ptr, ssize_t len)
{
	if (act == VDP_INIT){
		VSLb(req->vsl, SLT_Debug,"%s-S: RES", VMOD_DUMP_PRE);
		VSLb(req->vsl, SLT_Debug,"%s-V: %s", VMOD_DUMP_PRE, (char *)*priv);
		*priv = malloc(cache_param->vsl_reclen);
		AN(*priv);
		work_head(req, *priv, cache_param->vsl_reclen,  (void*)ptr, len, req->resp);
		return(0);
	}else if(act == VDP_FINI){
		VSLb(req->vsl, SLT_Debug,"%s-S: END", VMOD_DUMP_PRE);
		free(*priv);
		*priv = NULL;
		return(0);
	}

	if(len){
		work_body(req, *priv, cache_param->vsl_reclen,  (void*)ptr, len, req->resp);
	}
	return(VDP_bytes(req, act, ptr, len)); 
}
VCL_VOID
vmod_resp(VRT_CTX, VCL_STRING val)
{
	VDP_push(ctx->req, VDP_dump, (void*)val, 0);
}

