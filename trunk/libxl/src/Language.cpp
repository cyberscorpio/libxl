#include <assert.h>
#include "../include/Language.h"

XL_BEGIN

CLanguage::CLanguage () {

}

CLanguage::~CLanguage () {

}

CLanguage* CLanguage::getInstance () {
	static CLanguage language;
	return &language;
}

tstring CLanguage::getString (const tstring &key) {
	return key;
}


XL_END