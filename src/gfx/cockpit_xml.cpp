#include "cockpit.h"
#include "xml_support.h"
#include "gauge.h"
#include <float.h>
#include "hud.h"
#include "vdu.h"
#include "mesh.h"
using XMLSupport::EnumMap;
using XMLSupport::Attribute;
using XMLSupport::AttributeList;
using XMLSupport::parse_float;
using XMLSupport::parse_bool;
using XMLSupport::parse_int;
namespace CockpitXML {
    enum Names {
      UNKNOWN,
      COCKPIT,
      MESH,
      CROSSHAIRS,
      RADAR,
      LVDU,
      RVDU,
      AVDU,
      VDUTYPE,
      PANEL,
      ROWS,
      COLS,
      XFILE,
	  SOUNDFILE,
      XCENT,
      YCENT,
      TOPY,
      BOTY,
      XSIZE,
      YSIZE,
      MYFONT,
      RED,
      GREEN,
      BLUE,
      COCKPITOFFSET,
      VIEWOFFSET,
      FRONT,
      BACK,
      LEFT,
      RIGHT,
      KARMORF,
      KARMORB,
      KARMORR,
      KARMORL,
      KFUEL, 
      KSHIELDF,
      KSHIELDR,
      KSHIELDL,
      KSHIELDB, 
      KENERGY,
      KAUTO,
      KEJECT,
      KLOCK,
      KHULL,
      KKPS,
      KSETKPS,
      KFPS,
      G_UP,
      G_DOWN,
      G_LEFT,
      G_RIGHT,
	  COCKPIT_LAG
    };

  const EnumMap::Pair element_names[] = {
    EnumMap::Pair ("UNKNOWN", UNKNOWN),
    EnumMap::Pair ("Cockpit", COCKPIT),
    EnumMap::Pair ("Radar", RADAR),
    EnumMap::Pair ("LeftVDU", LVDU),
    EnumMap::Pair ("VDU", AVDU),
    EnumMap::Pair ("RightVDU", RVDU),
    EnumMap::Pair ("Panel", PANEL),
    EnumMap::Pair ("Crosshairs", CROSSHAIRS),
    EnumMap::Pair ("ArmorF", KARMORF),
    EnumMap::Pair ("ArmorR", KARMORR),
    EnumMap::Pair ("ArmorL", KARMORL),
    EnumMap::Pair ("ArmorB", KARMORB),
    EnumMap::Pair ("ShieldF", KSHIELDF),
    EnumMap::Pair ("ShieldR", KSHIELDR),
    EnumMap::Pair ("ShieldL", KSHIELDL),
    EnumMap::Pair ("ShieldB", KSHIELDB),
    EnumMap::Pair ("Fuel", KFUEL),
    EnumMap::Pair ("Energy", KENERGY),
    EnumMap::Pair ("Eject", KEJECT),
    EnumMap::Pair ("Lock", KLOCK),
    EnumMap::Pair ("Hull", KHULL),
    EnumMap::Pair ("Speed", KKPS),
    EnumMap::Pair ("SetSpeed", KSETKPS),
    EnumMap::Pair ("Auto", KAUTO),
    EnumMap::Pair ("FPS", KFPS)

  };
  const EnumMap::Pair attribute_names[] = {
    EnumMap::Pair ("UNKNOWN", UNKNOWN),
    EnumMap::Pair ("mesh", MESH),
    EnumMap::Pair ("file", XFILE),
    EnumMap::Pair ("soundfile", SOUNDFILE),
    EnumMap::Pair ("font", MYFONT),
    EnumMap::Pair ("front", FRONT),
    EnumMap::Pair ("left", LEFT),
    EnumMap::Pair ("right", RIGHT),
    EnumMap::Pair ("back", BACK),
    EnumMap::Pair ("xcent", XCENT),
    EnumMap::Pair ("ycent", YCENT),
    EnumMap::Pair ("width", XSIZE),
    EnumMap::Pair ("height", YSIZE),
    EnumMap::Pair ("Top", TOPY),
    EnumMap::Pair ("Bottom", BOTY),
    EnumMap::Pair ("ViewOffset", VIEWOFFSET),
    EnumMap::Pair ("CockpitOffset", COCKPITOFFSET),
    EnumMap::Pair ("GaugeUp",G_UP),
    EnumMap::Pair ("GaugeDown",G_DOWN),
    EnumMap::Pair ("GaugeLeft",G_LEFT),
    EnumMap::Pair ("GaugeRight",G_RIGHT),
    EnumMap::Pair ("TextRows", ROWS),
    EnumMap::Pair ("TextCols", COLS),
    EnumMap::Pair ("r", RED),
    EnumMap::Pair ("g", GREEN),
    EnumMap::Pair ("b", BLUE),
    EnumMap::Pair ("type", VDUTYPE),
    EnumMap::Pair ("lag", COCKPIT_LAG)
  };

  const EnumMap element_map(element_names, 25);
  const EnumMap attribute_map(attribute_names, 28);
}

using XMLSupport::EnumMap;
using XMLSupport::Attribute;
using XMLSupport::AttributeList;
using namespace CockpitXML;



void GameCockpit::beginElement(const string &name, const AttributeList &attributes) {
  AttributeList::const_iterator iter;
  Gauge::DIRECTION tmpdir=Gauge::GAUGE_UP;
  Sprite ** newsprite=NULL;
  VDU **newvdu=NULL;
  Sprite * adjsprite=NULL;
  std::string gaugename ("shieldstat.spr");
  std::string myfont ("9x12.font");
  Names elem = (Names)element_map.lookup(name);
  Names attr;
  unsigned int mymodes=0;
  float xsize=-1,ysize=-1,xcent=FLT_MAX,ycent=FLT_MAX;
  float leftx=-10;  float rightx=-10;
  float topy=-10; float boty = -10;
  short rows=13;
  short cols=15;
  switch (elem) {
  case COCKPIT:
    for(iter = attributes.begin(); iter!=attributes.end(); iter++) { 
      attr = (Names)attribute_map.lookup((*iter).name);
      switch (attr) {
      case MYFONT:
	myfont = (*iter).value;
	break;
      case RED:
	textcol.r = XMLSupport::parse_float ((*iter).value);
	break;
      case GREEN:
	textcol.g = XMLSupport::parse_float ((*iter).value);
	break;
      case BLUE:
	textcol.b = XMLSupport::parse_float ((*iter).value);
	break;
      case VIEWOFFSET:
		  viewport_offset = XMLSupport::parse_float ((*iter).value);
	break;
      case COCKPITOFFSET:
		  cockpit_offset = XMLSupport::parse_float ((*iter).value);
	break;
      case XFILE:
	Pit[0]= new Sprite ((*iter).value.c_str(),NEAREST);
	break;
      case SOUNDFILE:
	SetSoundFile((*iter).value);
	break;
      case MESH:
	mesh = new Mesh ((*iter).value.c_str(),1,0,NULL,false);
	break;
      case FRONT:
      case BACK:
      case LEFT:
      case RIGHT:
	Pit[attr-FRONT] = new Sprite ((*iter).value.c_str(),NEAREST);
	break;
      } 
    }
    text = new TextPlane ();
    break;
  case KARMORF:
  case KARMORR:
  case KARMORL:
  case KARMORB:
  case KFUEL:
  case KSHIELDF:
  case KSHIELDL:
  case KSHIELDR:
  case KSHIELDB:
  case KENERGY:
  case KEJECT:
  case KLOCK:
  case KHULL:
  case KKPS:
  case KSETKPS:
  case KAUTO:
  case KFPS:
    for(iter = attributes.begin(); iter!=attributes.end(); iter++) { 
      switch (attribute_map.lookup((*iter).name)) {
      case XFILE:
	gaugename = (*iter).value;
	break;
      case TOPY:
	topy = XMLSupport::parse_float ((*iter).value);
	break;
      case BOTY:
	boty = XMLSupport::parse_float ((*iter).value);
	break;
      case LEFT:
	leftx = XMLSupport::parse_float ((*iter).value);
	break;
      case RIGHT:
	rightx = XMLSupport::parse_float ((*iter).value);
	break;
      case XSIZE:
	xsize = XMLSupport::parse_float ((*iter).value);
	break;
      case YSIZE:
	ysize = XMLSupport::parse_float ((*iter).value);
	break;
      case XCENT:
	xcent = XMLSupport::parse_float ((*iter).value);
	break;
      case YCENT:
	ycent = XMLSupport::parse_float ((*iter).value);
	break;
      case G_UP:
	tmpdir = Gauge::GAUGE_UP;
	break;
      case G_DOWN:
	tmpdir = Gauge::GAUGE_DOWN;
	break;
      case G_LEFT:
	tmpdir = Gauge::GAUGE_LEFT;
	break;
      case G_RIGHT:
	tmpdir = Gauge::GAUGE_RIGHT;
	break;
      }
    }
    gauges[elem-KARMORF] = new Gauge (gaugename.c_str(), tmpdir);
    if (xsize!=-1) {
      gauges[elem-KARMORF]->SetSize (xsize,ysize);
    }
    if (xcent!=FLT_MAX) {
      gauges[elem-KARMORF]->SetPosition (xcent,ycent);
    }
    if (leftx!=-10&&rightx!=-10&&topy!=-10&&boty!=-10) {
      gauges[elem-KARMORF]->SetPosition (.5*(leftx+rightx),.5*(topy+boty));
      gauges[elem-KARMORF]->SetSize (fabs(leftx-rightx),fabs(topy-boty));
    }
    break;
  case CROSSHAIRS:
  case PANEL: 
    Panel.push_back (NULL);
    newsprite = &Panel.back ();
    if (elem==CROSSHAIRS) {
      Panel.back() = Panel.front();
      Panel.front()=NULL;//make sure null at the beginning
    }
    goto loadsprite;
  case RADAR: newsprite = &Radar;goto loadsprite;
  case LVDU: vdu.push_back(NULL);newvdu = &vdu.back();mymodes=VDU::MANIFEST|VDU::WEAPON|VDU::DAMAGE|VDU::SHIELD;goto loadsprite;
  case RVDU: vdu.push_back(NULL);newvdu = &vdu.back();mymodes=VDU::TARGETMANIFEST|VDU::NAV|VDU::TARGET;goto loadsprite;
  case AVDU:vdu.push_back(NULL);newvdu = &vdu.back();mymodes=VDU::MSG;
    for(iter = attributes.begin(); iter!=attributes.end(); iter++) { 
      switch (attribute_map.lookup((*iter).name)) {
      case VDUTYPE:
	mymodes=parse_vdu_type ((*iter).value.c_str());
	break;
      default:
	break;
      }
    }
    goto loadsprite;
  loadsprite:
    for(iter = attributes.begin(); iter!=attributes.end(); iter++) { 
      switch (attribute_map.lookup((*iter).name)) {
      case XFILE:
	if (newsprite) {
	  (*newsprite) = new Sprite ((*iter).value.c_str(),NEAREST);
	  adjsprite = *newsprite;
	} else if (newvdu) {
	  (*newvdu) = new VDU ((*iter).value.c_str(),text,mymodes,rows,cols,&StartArmor[0],&maxhull);
	  adjsprite = *newvdu;
	}
	break;
      case TOPY:
	topy = XMLSupport::parse_float ((*iter).value);
	break;
      case BOTY:
	boty = XMLSupport::parse_float ((*iter).value);
	break;
      case LEFT:
	leftx = XMLSupport::parse_float ((*iter).value);
	break;
      case RIGHT:
	rightx = XMLSupport::parse_float ((*iter).value);
	break;
      case XSIZE:
	xsize = XMLSupport::parse_float ((*iter).value);
	break;
      case YSIZE:
	ysize = XMLSupport::parse_float ((*iter).value);
	break;
      case XCENT:
	xcent = XMLSupport::parse_float ((*iter).value);
	break;
      case YCENT:
	ycent = XMLSupport::parse_float ((*iter).value);
	break;
      case ROWS:
	rows =  XMLSupport::parse_int ((*iter).value);
	break;
      case COLS:
	cols = XMLSupport::parse_int ((*iter).value);
	break;
	break;

      }
    }
    if (adjsprite) {
      if (xsize!=-1) {
	adjsprite->SetSize (xsize,ysize);
      }
      if (xcent!=FLT_MAX) {
	adjsprite->SetPosition (xcent,ycent);
      }
      if (leftx!=-10&&rightx!=-10&&topy!=-10&&boty!=-10) {
	adjsprite->SetPosition (.5*(leftx+rightx),.5*(topy+boty));
	adjsprite->SetSize (fabs(leftx-rightx),fabs(topy-boty));
      }
    } else {
      if (newsprite==&Panel.back()) {
	Panel.erase (Panel.end()-1);//don't want null panels
      }
    }
    break;

  }
  
}
void GameCockpit::endElement(const string &name) {

}



void GameCockpit::LoadXML (const char * filename) {
  const int chunk_size = 16384;

  FILE * inFile=NULL;

  if (filename[0]!='\0') {
	inFile = fopen (filename, "r");

  }
  if(!inFile) {
	 cockpit_offset=0;
	 viewport_offset=0;
	 Panel.push_back(new Sprite ("crosshairs.spr"));
	 return;
	

  }
  XML_Parser parser = XML_ParserCreate(NULL);
  XML_SetUserData(parser, this);
  XML_SetElementHandler(parser, &Cockpit::beginElement, &Cockpit::endElement);
  
  do {
#ifdef BIDBG
    char *buf = (XML_Char*)XML_GetBuffer(parser, chunk_size);
#else
    char buf[chunk_size];
#endif
    int length;
    
    length = fread (buf,1, chunk_size,inFile);
    //length = inFile.gcount();
#ifdef BIDBG
    XML_ParseBuffer(parser, length, feof(inFile));
#else
    XML_Parse (parser,buf,length,feof(inFile));
#endif
  } while(!feof(inFile));
  fclose (inFile);
  XML_ParserFree (parser);
}
