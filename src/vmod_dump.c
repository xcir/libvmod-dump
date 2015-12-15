#include <stdio.h>
#include <stdlib.h>

#include "vcl.h"
#include "vrt.h"
#include "vrt_obj.h"
#include <sys/socket.h>
#include "vsa.h"
#include "cache/cache.h"
#include "cache/cache_filter.h"

#include "vcc_if.h"

#define VMOD_DUMP_PRE      "VMD-DUMP"
#define VMOD_DUMP_KEY      "VMD-DUMP: "
#define VMOD_DUMP_KEY_LEN  10

//from varnishd/cache_http1_proto.c
const int HTTP1_Req[3] = {
	HTTP_HDR_METHOD, HTTP_HDR_URL, HTTP_HDR_PROTO
};

const int HTTP1_Resp[3] = {
	HTTP_HDR_PROTO, HTTP_HDR_STATUS, HTTP_HDR_REASON
};


void dump_VSL_split(struct req *req, char *p, unsigned mlen, const void *ptr, size_t l, char * suf){
	
	char *p2       = p + VMOD_DUMP_KEY_LEN;
	unsigned mlen2 = mlen - VMOD_DUMP_KEY_LEN;
	size_t length  = l;
	void *c_ptr    = (void*)ptr;
	txt t;
	t.b = p;
	unsigned slen = strlen(suf);
	assert(slen <= 2);
	do{
		if(mlen2 > length){
			memcpy(p2, c_ptr, length);
			t.e = t.b + length + VMOD_DUMP_KEY_LEN;
			if(slen > 0) {
				memcpy(p2 + length, suf, slen);
				t.e += slen;
			}
			VSLbt(req->vsl, SLT_Debug,t);
			break;
		}
		memcpy(p2, c_ptr, mlen2);
		t.e = t.b + mlen2 + VMOD_DUMP_KEY_LEN;
		if(slen > 0) {
			memcpy(p2 + mlen2, suf, slen);
			t.e += slen;
		}
		VSLbt(req->vsl, SLT_Debug,t);
		c_ptr  += mlen2;
		length -= mlen2;
		
	}while(1);
}

int work_head(struct req *req, char *p,unsigned plen, struct http *http,const int *hf)
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
	
	//header
	AN(http->hd[hf[0]].b);
	AN(http->hd[hf[1]].b);
	AN(http->hd[hf[2]].b);
	dump_VSL_split(req, p, mlen, http->hd[hf[0]].b, (http->hd[hf[0]].e - http->hd[hf[0]].b), " ");
	dump_VSL_split(req, p, mlen, http->hd[hf[1]].b, (http->hd[hf[1]].e - http->hd[hf[1]].b), " ");
	dump_VSL_split(req, p, mlen, http->hd[hf[2]].b, (http->hd[hf[2]].e - http->hd[hf[2]].b), "\r\n");
	
	for(int i=HTTP_HDR_FIRST; i < http->nhd; ++i) {
		if (http->hd[i].b == NULL && http->hd[i].e == NULL)	continue;
		dump_VSL_split(req, p, mlen, http->hd[i].b, (http->hd[i].e - http->hd[i].b), "\r\n");
	}
	dump_VSL_split(req,p,mlen,"\r\n",2,"");
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
		dump_VSL_split(req, p, mlen, ptr, l, "");
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
	work_body(req,req->ws->f, u, ptr, l, req->http0);
	
	//free work-space
	WS_Release(req->ws, 0);
	return(0);
}

VCL_VOID
vmod_req(VRT_CTX, VCL_STRING val)
{
	//thread check
	if(ctx->req == NULL || ctx->req->magic != REQ_MAGIC){
		VSLb(ctx->vsl, SLT_Error,"vmod-dump: dump.req work only at client-thread.");
		return;
	}
	//remote client local server
	//VRT_r_remote_ip
	//VRT_r_client_ip
	//VRT_r_local_ip
	//VRT_r_server_ip
	//VRT_IP_string(IP) ip->str
	//VSA_Port(IP) ip->port
	VSLb(ctx->vsl, SLT_Debug,"%s-I: %s %d %s %d", VMOD_DUMP_PRE,
		VRT_IP_string(ctx, VRT_r_client_ip(ctx)),
		VSA_Port(VRT_r_client_ip(ctx)),
		VRT_IP_string(ctx, VRT_r_server_ip(ctx)),
		VSA_Port(VRT_r_server_ip(ctx))
	);
	VSLb(ctx->vsl, SLT_Debug,"%s-S: REQ", VMOD_DUMP_PRE);
	VSLb(ctx->vsl, SLT_Debug,"%s-V: %s", VMOD_DUMP_PRE, val);
	unsigned u;
	//reserve work-space
	u = WS_Reserve(ctx->req->ws, 0);
	if(u <= VMOD_DUMP_KEY_LEN) {
		//no space.
		WS_Release(ctx->req->ws, 0);
		return;
	}
	work_head(ctx->req,ctx->req->ws->f, u, ctx->req->http0,HTTP1_Req);
	//free work-space
	WS_Release(ctx->req->ws, 0);
	
	VRB_Iterate(ctx->req, vbf_printRequestBody, NULL);
	VSLb(ctx->vsl, SLT_Debug,"%s-S: END", VMOD_DUMP_PRE);
}

int __match_proto__(vdp_bytes)
VDP_dump(struct req *req, enum vdp_action act, void **priv,
    const void *ptr, ssize_t len)
{
	//priv fmt
	// flg(Complete the header output)@1Byte + [workspace]
	if (act == VDP_INIT){
		VSLb(req->vsl, SLT_Debug,"%s-S: RES", VMOD_DUMP_PRE);
		VSLb(req->vsl, SLT_Debug,"%s-V: %s", VMOD_DUMP_PRE, (char *)*priv);
		*priv = malloc(cache_param->vsl_reclen + 1);
		AN(*priv);
		*((char **)priv)[0] = 0;
		return(0);
	}else if(act == VDP_FINI){
		VSLb(req->vsl, SLT_Debug,"%s-S: END", VMOD_DUMP_PRE);
		free(*priv);
		*priv = NULL;
		return(0);
	}

	if(len){
		if(*((char **)priv)[0] == 0){
			work_head(req, *priv+1, cache_param->vsl_reclen,  req->resp,HTTP1_Resp);
			*((char **)priv)[0] = 1;
		}
		work_body(req, *priv+1, cache_param->vsl_reclen,  (void*)ptr, len, req->resp);
	}
	return(VDP_bytes(req, act, ptr, len)); 
}
VCL_VOID
vmod_resp(VRT_CTX, VCL_STRING val)
{
	//thread check
	if(ctx->req == NULL || ctx->req->magic != REQ_MAGIC){
		VSLb(ctx->vsl, SLT_Error,"vmod-dump: dump.resp work only at client-thread.");
		return;
	}
	
	VSLb(ctx->vsl, SLT_Debug,"%s-I: %s %d %s %d", VMOD_DUMP_PRE,
		VRT_IP_string(ctx, VRT_r_client_ip(ctx)),
		VSA_Port(VRT_r_client_ip(ctx)),
		VRT_IP_string(ctx, VRT_r_server_ip(ctx)),
		VSA_Port(VRT_r_server_ip(ctx))
	);
	VDP_push(ctx->req, VDP_dump, (void*)val, 1);
}
