defstar {
  name { WriteVar }
  domain { SDF }
  derivedfrom {SDFSharedMem}
  desc {}
  input
  {
    name { in }
    type { float }
  }
  state
  {
    name { name }
    type { string }
    default { "0" }
    desc { Name of the register }
  }
  go
  {
    float val;

    val = float(in%0);
    SDFSharedMem::WriteVar(name,val);
  }
}
