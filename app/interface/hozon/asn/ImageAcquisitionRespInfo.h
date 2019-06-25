/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "HOZON"
 * 	found in "HOZON_PRIV_v1.0.asn"
 * 	`asn1c -gen-PER`
 */

#ifndef	_ImageAcquisitionRespInfo_H_
#define	_ImageAcquisitionRespInfo_H_


#include <asn_application.h>

/* Including external dependencies */
#include <BOOLEAN.h>
#include <NativeInteger.h>
#include <IA5String.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ImageAcquisitionRespInfo */
typedef struct ImageAcquisitionRespInfo {
	BOOLEAN_t	 result;
	long	*failureType	/* OPTIONAL */;
	IA5String_t	*fileName	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} ImageAcquisitionRespInfo_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ImageAcquisitionRespInfo;

#ifdef __cplusplus
}
#endif

#endif	/* _ImageAcquisitionRespInfo_H_ */
#include <asn_internal.h>