/* octIfc.h  edg
Version identification:
$Id$
*/

extern char *AkoName();

extern boolean SetFormalParams();
extern boolean GetFormalParams();
extern boolean SetSogParams();
extern boolean GetOrInitSogParams();
extern void GetOrInitDelayProp();
extern void GetOrInitBusProp();
extern void IntizeProp();
extern boolean IsVemConnector();
extern boolean IsIoPort();
extern boolean IsInputPort();
extern boolean IsDelay();
extern boolean IsGal();
extern boolean IsStar();
extern boolean IsUniv();
extern boolean IsGalFacet();
extern boolean GetGalTerms();
extern boolean GetHardwareProp();
extern boolean SetHardwareProp();
extern boolean GetCommentProp();
extern boolean SetCommentProp();
extern boolean GOCDomainProp();
extern boolean GOCArchProp();
extern int GetIterateProp();
extern void SetIterateProp();
