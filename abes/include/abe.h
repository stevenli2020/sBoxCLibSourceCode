/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef ABE_H
#define ABE_H

#ifdef __cplusplus
extern "C" {
#endif

	#include "pbc.h"
	#include "glib.h"
	#include "pbc_sig.h"

	#include "ec/ecsigkey.h"
	#include "ec/ecsiggen.h"

	#include "pairing/pairing_type.h"
	#include "pairing/group_context.h"
	#include "pairing/pairing_group.h"
	#include "pairing/pairing_factory.h"

	#include "util/abe_util.h"
	#include "util/common.h"
	#include "util/io.h"
 
	#include "base/base_obj.h"
	#include "base/base_free.h"
	#include "base/base_serializer.h"
	#include "base/base.h"

	#include "odabe/odabe_obj.h"
	#include "odabe/odabe_free.h"
	#include "odabe/odabe_serializer.h"
	#include "odabe/odabe.h"
	
	#include "cca_odabe/cca_odabe_obj.h"
	#include "cca_odabe/cca_odabe_free.h"
	#include "cca_odabe/cca_odabe_serializer.h"
	#include "cca_odabe/cca_odabe.h"

	#include "cca_eodabe/cca_eodabe_obj.h"
	#include "cca_eodabe/cca_eodabe_free.h"
	#include "cca_eodabe/cca_eodabe_serializer.h"
	#include "cca_eodabe/cca_eodabe.h"

#ifdef __cplusplus
}
#endif

#endif /* ABE_H */

