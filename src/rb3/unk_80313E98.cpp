#include "data.hpp"
#include "symbol.hpp"
#include "std/string.h"
#include "std/stdlib.h"
#include "common.hpp"

extern char *lbl_8091A47C;
extern int lbl_8091A480;
extern "C" char *fn_80315C3C(int);

#pragma dont_inline on
// fn_80315C3C
char *fn_80315C3C(int i)
{
	lbl_8091A480 += 1;
	char *old = lbl_8091A47C;
	lbl_8091A47C += i;
	return old;
}

extern bool lbl_808E4468;
extern void *MemOrPoolAlloc(int, int);

DataNode *NodesAlloc(int i)
{
	if (lbl_808E4468)
		return (DataNode *)fn_80315C3C(i);
	else
		return (DataNode *)MemOrPoolAlloc(i, 1);
}

extern char *lbl_8091A478; // 4 bytes long

// this probably isn't a pointer
extern char *lbl_8091A484; // 0x14 bytes long
extern "C" bool fn_80315C7C(void *);

// fn_80315C7C

// Checks if v is within a memory region
bool fn_80315C7C(void *v)
{
	return (v >= lbl_8091A478) && (v < &lbl_8091A478[(int)lbl_8091A484]);
}

extern void MemOrPoolFree(int, int, void *);

void NodesFree(int i, DataNode *dn)
{
	fn_80315C7C(dn);
	MemOrPoolFree(i, 1, dn);
}
#pragma dont_inline reset

extern int gIndent;

// fn_80315A70
void DataArray::Print(TextStream &ts, DataType ty, bool b) const
{
	DataNode *lol;
	DataNode *dn = mNodes;
	DataNode *dn_end = &mNodes[mNodeCount];
	char begin = '\0';
	char end = '\0';
	if (ty == kDataArray) {
		begin = '(';
		end = ')';
	} else if (ty == kDataCommand) {
		begin = '{';
		end = '}';
	} else if (ty == kDataProperty) {
		begin = '[';
		end = ']';
	}

	while (dn < dn_end) {
		if (dn->GetType() & 0x10)
			break;
		dn++;
	}

	if ((dn != dn_end) && !b) {
		ts << begin;
		lol = mNodes;
		if (lol->GetType() == kDataObject) {
			lol->Print(ts, b);
			lol++;
		}
		ts << " ";
		gIndent += 3;
		while (lol < dn_end) {
			ts.Space(gIndent);
			lol->Print(ts, b);
			ts << " ";
			lol++;
		}
		gIndent -= 3;
		ts.Space(gIndent);
		ts << end;
	} else {
		ts << begin;
		for (lol = mNodes; lol < dn_end; lol++) {
			if (lol != mNodes)
				ts << "\n";
			lol->Print(ts, b);
		}
		ts << end;
	}
}

extern DataNode *NodesAlloc(int);

DataArray::DataArray(int i)
	: symbol(), mNodeCount(i), mRefCount(1), mLine(0), mUnknown(0)
{
	mNodes = NodesAlloc(i * sizeof(DataNode));
	for (int n = 0; n < i; n++) {
		new (&mNodes[n]) DataNode();
	}
}

DataArray::DataArray(const void *v, int i)
	: symbol(), mNodeCount(-i), mRefCount(1), mLine(0), mUnknown(0)
{
	mNodes = NodesAlloc(i);
	memcpy(mNodes, v, i);
}

extern void NodesFree(int, DataNode *);

DataArray::~DataArray()
{
	if (mNodeCount < 0)
		NodesFree(-mNodeCount, mNodes);
	else {
		for (int i = 0; i < mNodeCount; i++) {
			mNodes[i].~DataNode();
		}
		NodesFree(mNodeCount * sizeof(DataNode), mNodes);
	}
}

int NodeCmp(const void *a, const void *b)
{
	DataNode *da = (DataNode *)a;
	DataNode *db = (DataNode *)b;
	switch (da->GetType()) {
	case kDataFloat:
	case kDataInt:
		double d1 = da->LiteralFloat(nullptr);
		double d2 = db->LiteralFloat(nullptr);
		if (d1 < d2)
			return -1;
		return (d1 != d2);
	case kDataString:
	case kDataObject:
		const char *s1 = da->Str(nullptr);
		const char *s2 = db->Str(nullptr);
		return stricmp(s1, s2);
	default:
		return 0;
	}
}

void DataArray::SortNodes()
{
	if (mNodeCount <= 0)
		return;
	qsort(mNodes, mNodeCount, 8, NodeCmp);
}

// fn_80317B18
void DataArray::SaveGlob(BinStream &bs, bool b) const
{
	if (b) {
		int i = -1 - mNodeCount;
		bs << (unsigned int)i;
		bs.Write(mNodes, i);
	} else {
		bs << mNodeCount;
		bs.Write(mNodes, -mNodeCount);
	}
}

// fn_80317B9C
void DataArray::LoadGlob(BinStream &bs, bool b)
{
	unsigned int v;
	NodesFree(-mNodeCount, mNodes);
	if (b) {
		bs >> v;
		mNodeCount = -(v + 1);
		mNodes = NodesAlloc(-mNodeCount);
		bs.Read(mNodes, v);
	} else {
		bs >> mNodeCount;
		mNodes = NodesAlloc(-mNodeCount);
		bs.Read(mNodes, -mNodeCount);
	}
}

// fn_80316CB0
void DataArray::SetFileLine(Symbol s, int i)
{
	symbol = s;
	mLine = i;
}

// fn_80315CFC
void DataArray::Insert(int count, const DataNode &dn)
{
	int i = 0;
	int newNodeCount = mNodeCount + 1;
	DataNode *oldNodes = mNodes; // Save all nodes pointer
	// allocate new nodes
	mNodes = NodesAlloc(newNodeCount * sizeof(DataNode));

	for (i = 0; i < count; i++) {
		new (&mNodes[i]) DataNode(oldNodes[i]);
	}
	for (; i < count + 1; i++) {
		new (&mNodes[i]) DataNode(dn);
	}
	for (; i < newNodeCount; i++) {
		new (&mNodes[i]) DataNode(oldNodes[i - 1]);
	}
	for (i = 0; i < mNodeCount; i++) {
		oldNodes[i].~DataNode();
	}

	// free old nodes
	NodesFree(mNodeCount * sizeof(DataNode), oldNodes);
	mNodeCount = newNodeCount;
}

extern "C" void fn_80315E1C(DataArray *, int, DataArray *);

// fn_80315E1C
void DataArray::InsertNodes(int count, const DataArray *da)
{
	if ((da == 0) || (da->GetNodeCount() == 0))
		return;
	int i = 0;
	int dacnt = da->GetNodeCount();
	int newNodeCount = mNodeCount + dacnt;
	DataNode *oldNodes = mNodes; // Save all nodes pointer
	// allocate new nodes
	mNodes = NodesAlloc(newNodeCount * sizeof(DataNode));

	for (i = 0; i < count; i++) {
		new (&mNodes[i]) DataNode(oldNodes[i]);
	}

	for (; i < count + dacnt; i++) {
		new (&mNodes[i]) DataNode(*da->GetNodeAtIndex(i - count));
	}

	for (; i < newNodeCount; i++) {
		new (&mNodes[i]) DataNode(oldNodes[i - dacnt]);
	}
	for (i = 0; i < mNodeCount; i++) {
		oldNodes[i].~DataNode();
	}
	NodesFree(mNodeCount * sizeof(DataNode), oldNodes);
	mNodeCount = newNodeCount;
}

// fn_80315F74
void DataArray::Resize(int i)
{
	DataNode *oldNodes = mNodes;
	mNodes = NodesAlloc(i * sizeof(DataNode));
	int min = Minimum(mNodeCount, i);
	int cnt = 0;
	for (cnt = 0; cnt < min; cnt++) {
		new (&mNodes[cnt]) DataNode(oldNodes[cnt]);
	}
	for (; cnt < i; cnt++) {
		new (&mNodes[cnt]) DataNode();
	}
	for (cnt = 0; cnt < mNodeCount; cnt++) {
		oldNodes[cnt].~DataNode();
	}
	NodesFree(mNodeCount * sizeof(DataNode), oldNodes);
	mNodeCount = i;
	mUnknown = 0;
}

// fn_80316064
void DataArray::Remove(int i)
{
	DataNode *oldNodes = mNodes;
	int newCnt = mNodeCount - 1;
	mNodes = NodesAlloc(newCnt * sizeof(DataNode));
	int cnt = 0;
	for (cnt = 0; cnt < i; cnt++) {
		new (&mNodes[cnt]) DataNode(oldNodes[cnt]);
	}
	for (; i < newCnt; i++) {
		new (&mNodes[i]) DataNode(oldNodes[i + 1]);
	}
	for (int j = 0; j < mNodeCount; j++) {
		oldNodes[j].~DataNode();
	}
	NodesFree(mNodeCount * sizeof(DataNode), oldNodes);
	mNodeCount = newCnt;
}

// fn_80316150
void DataArray::Remove(const DataNode &dn)
{
	int searchType = dn.value.intVal;
	for (int lol = mNodeCount - 1; lol >= 0; lol--) {
		if (mNodes[lol].value.intVal == searchType) {
			Remove(lol);
			return;
		}
	}
}

// fn_80316190
bool DataArray::Contains(const DataNode &dn) const
{
	int searchType = dn.value.intVal;
	for (int lol = mNodeCount - 1; lol >= 0; lol--) {
		if (mNodes[lol].value.intVal == searchType) {
			return true;
		}
	}
	return false;
}

// fn_803161D4 - https://decomp.me/scratch/KWNxW

#pragma dont_inline on
// fn_80317278
BinStream &operator<<(BinStream &bs, const DataNode *dn)
{
	dn->Save(bs);
	return bs;
}
#pragma dont_inline reset

// fn_803171F8
void DataArray::Save(BinStream &bs) const
{
	bs << mNodeCount << mLine << mUnknown;
	for (int i = 0; i < mNodeCount; i++) {
		bs << &mNodes[i];
	}
}

// fn_80317AE0
BinStream &operator>>(BinStream &bs, DataNode *dn)
{
	dn->Load(bs);
	return bs;
}

// fn_80317E5C
TextStream &operator<<(TextStream &ts, const DataArray *da)
{
	if (da != nullptr)
		da->Print(ts, kDataArray, false);
	else
		ts << "<null>";
	return ts;
}

extern DataArray *fn_8035CF9C(int, int, int);

// fn_80317EB8
BinStream &operator>>(BinStream &bs, DataArray *&da)
{
	bool b;
	bs >> b;
	if (b) {
		da = new (fn_8035CF9C(0x10, 0x10, 1)) DataArray(0);
		da->Load(bs);
	} else
		da = nullptr;
	return bs;
}

// fn_80317F3C
BinStream &operator<<(BinStream &bs, const DataArray *da)
{
	if (da != nullptr) {
		bs << (char)1;
		da->Save(bs);
	} else
		bs << (char)0;
	return bs;
}