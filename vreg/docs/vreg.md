# クラス関係

```puml

interface VMountBase{
  std::string name, desc
  size_t? writeAt(addr_t,span<const byte>)
  size_t? readAt(addr_t,span<byte>)
  size_t size()
}

class VMap{
  vector<pair<addr_t, VMountBase*>> map;
}

interface VRegBase

class VRange{
  vector<VRegBase*> range
}

rectangle VReg{
  class VRegRW
  class VRegWO
  class VRegRO
}

rectangle VRegBinder{
  class VRegBinderRW
  class VRegBinderRO
}


'Relation
VRegBase -u-|>VMountBase

VMap-l-*VMountBase 
VMap-l-|>VMountBase 

VRange-r-|>VMountBase
VRange-r-*VRegBase 

VReg ---u-|>VRegBase
VRegBinder ---u-|>VRegBase

```

## 
