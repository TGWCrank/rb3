#include "ui/Screenshot.h"
#include "rndobj/Tex.h"
#include "rndobj/Mat.h"
#include "obj/PropSync_p.h"
#include "utl/Symbols.h"

unsigned short Screenshot::gRev = 0;
unsigned short Screenshot::gAltRev = 0;

BEGIN_COPYS(Screenshot)
    COPY_SUPERCLASS(Hmx::Object)
    COPY_SUPERCLASS(RndDrawable)
    GET_COPY(Screenshot)
    if(c && ty != kCopyFromMax) COPY_MEMBER(mTexPath)
    Sync();
END_COPYS

SAVE_OBJ(Screenshot, 0x2D)

void Screenshot::Load(BinStream& bs){
    LOAD_REVS(bs);
    ASSERT_REVS(1, 0);
    Hmx::Object::Load(bs);
    RndDrawable::Load(bs);
}

void Screenshot::Sync(){
    
}

Screenshot::~Screenshot(){
    delete mTex;
    delete mMat;
}

Screenshot::Screenshot() : mTex(0), mMat(0) {

}

BEGIN_HANDLERS(Screenshot)
    HANDLE_SUPERCLASS(RndDrawable)
    HANDLE_SUPERCLASS(Hmx::Object)
    HANDLE_CHECK(0x85)
END_HANDLERS

BEGIN_PROPSYNCS(Screenshot)
    // SYNC_PROP_ACTION(tex_path, mTexPath, kPropSize|kPropGet, Sync())
    // TODO: fix the SYNC_PROP_ACTION macro to incorporate this order of control flow, while not breaking the other SyncProps
    if(sym == tex_path){ 
        bool synced = PropSync(mTexPath, _val, _prop, _i + 1, _op);
        if(synced) { 
            if(!(_op & (kPropSize|kPropGet))){ Sync(); } 
            return true; 
        } 
        else return false; 
    }
    SYNC_SUPERCLASS(RndDrawable)
END_PROPSYNCS