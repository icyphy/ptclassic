defstar {
  name { ReadVar }
  domain { SDF }
  derivedfrom {SDFSharedMem}
  desc {}
  output
  {
    name { out }
    type { float }
  }
  state
  {
    name { name }
    type { string }
    default { "0" }
    desc { Name of the register }
  }
  state
  {
    name { initial }
    type { float }
    default { 0 }
    desc { Default value for output }
  }
  go
  {
    float val;

    if (!SDFSharedMem::ReadVar(name,val))
    {
      val = float(initial);
    }
    out%0 << val;
  }
  wrapup
  {
    SDFSharedMem::RemoveVar(name);
  }
}
