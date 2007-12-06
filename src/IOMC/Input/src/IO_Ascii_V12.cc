//--------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// Matt.Dobbs@Cern.CH, January 2000
// event input/output in ascii format for machine reading
//////////////////////////////////////////////////////////////////////////

//#include "HepMC/IO_Ascii.h"
#include "IOMC/Input/interface/IO_Ascii_V12.h"
#include "HepMC/GenEvent.h"
#include "HepMC/ParticleDataTable.h"
#include<cassert>

namespace HepMC {

    IO_Ascii_V12::IO_Ascii_V12( const char* filename, std::ios::openmode mode ) 
	: m_mode(mode), m_file(filename, mode), m_finished_first_event_io(0) 
    {
	if ( (m_mode&std::ios::out && m_mode&std::ios::in) ||
	     (m_mode&std::ios::app && m_mode&std::ios::in) ) {
	    std::cerr << "IO_Ascii_V12::IO_Ascii_V12 Error, open of file requested "
		      << "of input AND output type. Not allowed. Closing file."
		      << std::endl;
	    m_file.close();
	}
	// precision 16 (# digits following decimal point) is the minimum that
	//  will capture the full information stored in a double
	m_file.precision(16);
	// we use decimal to store integers, because it is smaller than hex!
	m_file.setf(std::ios::dec,std::ios::basefield);
	m_file.setf(std::ios::scientific,std::ios::floatfield);
        // now set pointers to input and output objects
        m_iostr = & m_file;
        if (m_mode&std::ios::in) {
          m_istr = & m_file;
          m_ostr = NULL;
        } else {
          m_ostr = & m_file;
          m_istr = NULL;
        }
        m_have_file = true;
    }

  IO_Ascii_V12::IO_Ascii_V12( std::istream * istr ) :  m_finished_first_event_io(0) {
      m_istr  = istr;
      m_iostr = istr;
      m_ostr  = NULL;
      m_have_file = false;
    }

  IO_Ascii_V12::IO_Ascii_V12( std::ostream * ostr ) :  m_finished_first_event_io(0) {
      m_ostr  = ostr;
      m_iostr = ostr;
      m_istr  = NULL;
      m_have_file = false;
      //  will capture the full information stored in a double
      m_ostr->precision(16);
      // we use decimal to store integers, because it is smaller than hex!
      m_ostr->setf(std::ios::dec,std::ios::basefield);
      m_ostr->setf(std::ios::scientific,std::ios::floatfield);
    }


    IO_Ascii_V12::~IO_Ascii_V12() {
	write_end_listing();
	if (m_have_file) m_file.close();
    }

    void IO_Ascii_V12::print( std::ostream& ostr ) const { 
      ostr << "IO_Ascii_V12: unformated ascii file IO for machine reading.\n" ;
      if (m_have_file) ostr << "\tFile openmode: " << m_mode;
      ostr   << " stream state: " << m_iostr->rdstate()
	     << " bad:" << (m_iostr->rdstate()&std::ios::badbit)
	     << " eof:" << (m_iostr->rdstate()&std::ios::eofbit)
	     << " fail:" << (m_iostr->rdstate()&std::ios::failbit)
	     << " good:" << (m_iostr->rdstate()&std::ios::goodbit) << std::endl;
    }

    void IO_Ascii_V12::write_event( const GenEvent* evt ) {
	/// Writes evt to m_file. It does NOT delete the event after writing.
	//
	// check the state of m_ is good, and that it is in output mode
	if ( !evt  ) return;
	if ( (m_ostr == NULL) ) {
	    std::cerr << "HepMC::IO_Ascii_V12::write_event "
		      << " attempt to write to input file." << std::endl;
	    return;
	}
	//
	// write event listing key before first event only.
	if ( !m_finished_first_event_io ) {
	    m_finished_first_event_io = 1;
	    *m_ostr << "\n" << "HepMC::IO_Ascii-START_EVENT_LISTING\n";
	}
	//
	// output the event data including the number of primary vertices
	//  and the total number of vertices
	std::vector<long int> random_states = evt->random_states();
	*m_ostr << 'E';
	output( evt->event_number() );
	output( evt->event_scale() );
	output( evt->alphaQCD() );
	output( evt->alphaQED() );
	output( evt->signal_process_id() );
	output(   ( evt->signal_process_vertex() ?
		    evt->signal_process_vertex()->barcode() : 0 )   );
	output( evt->vertices_size() ); // total number of vertices.
	output( (int)random_states.size() );
	for ( std::vector<long int>::iterator rs = random_states.begin(); 
	      rs != random_states.end(); ++rs ) {
	    output( *rs );
	}
	output( (int)evt->weights().size() );
	for ( WeightContainer::const_iterator w = evt->weights().begin(); 
	      w != evt->weights().end(); ++w ) {
	    output( *w );
	}
	output('\n');
	//
	// Output all of the vertices - note there is no real order.
	for ( GenEvent::vertex_const_iterator v = evt->vertices_begin();
	      v != evt->vertices_end(); ++v ) {
	    write_vertex( *v );
	}
    }

    bool IO_Ascii_V12::fill_next_event( GenEvent* evt ){
	//
	//
	// test that evt pointer is not null
	if ( !evt ) {
	    std::cerr 
		<< "IO_Ascii_V12::fill_next_event error - passed null event." 
		<< std::endl;
	    return 0;
	}
	// check the state of m_file is good, and that it is in input mode
	if ( !(*m_istr) ) return 0;
	if ( !m_istr) {
	    std::cerr << "HepMC::IO_Ascii_V12::fill_next_event "
		      << " attempt to read from output file." << std::endl;
	    return 0;
	}
	//
	// search for event listing key before first event only.
	//
	// skip through the file just after first occurence of the start_key
	if ( !m_finished_first_event_io ) {
          // comment out the following since user may want to provide
          // a stream at any point in the game?
          //m_istr->seekg( 0 ); // go to position zero in the file.
          //
          // establish whether we have a tag corresponding to either of our
          // 2 format versions
          m_format_version = search_for_2key_end( *m_istr,
                                                  "BlockType GenEvent\n",
                                                  "HepMC::IO_Ascii-START_EVENT_LISTING\n");
          if (m_format_version == 0) {
            std::cerr << "IO_Ascii_V12::fill_next_event start key not found "
                      << "setting badbit." << std::endl;
            m_istr->clear(std::ios::badbit); 
            return 0;
          }
          m_finished_first_event_io = 1;
	}
	//
	// test to be sure the next entry is of type "E" then ignore it.
        // If it is not "E" then look for alternative tag according to
        // format version
	if ( !(*m_istr) || m_istr->peek()!='E' ) { 
	    // if the E is not the next entry, then check to see if it is
	    // the end event listing key - if yes, search for another start key
          if (m_format_version == 2) {
	    if ( eat_key((*m_istr), "HepMC::IO_Ascii-END_EVENT_LISTING\n") ) {
		bool search_result = search_for_key_end((*m_istr),
				      "HepMC::IO_Ascii-START_EVENT_LISTING\n");
		if ( !search_result ) {
		    // this is the only case for format version 1 where we set an EOF state
		    m_istr->clear(std::ios::eofbit);
		    return 0;
		}
	    } else {
		std::cerr << "IO_Ascii_V12::fill_next_event end key not found "
			  << "setting badbit." << std::endl;
		m_istr->clear(std::ios::badbit); 
		return 0;
	    }
          } else if (m_format_version == 1) {
            if (! search_for_key_end(*m_istr, "BlockType GenEvent\n") ) {
              // this is the only case for format version 1 where we set an EOF state
              m_istr->clear(std::ios::eofbit);
              return 0;
            }
          } else {
            std::cerr << "IO_Ascii_V12::fill_next_event encountered internal error "
                      << "(unrecognised format version " << m_format_version << ")" << std::endl;
            return 0;
          }
	} 
	m_istr->ignore();
        //std::cerr << "Reading E" << std::endl;
	// read values into temp variables, then create a new GenEvent
	int event_number = 0, signal_process_id = 0, signal_process_vertex = 0,
	    num_vertices = 0, random_states_size = 0, weights_size = 0;
	double eventScale = 0, alpha_qcd = 0, alpha_qed = 0;
	*m_istr >> event_number >> eventScale >> alpha_qcd >> alpha_qed
	       >> signal_process_id >> signal_process_vertex
	       >> num_vertices >> random_states_size;
	std::vector<long int> random_states(random_states_size);
	for ( int i = 0; i < random_states_size; ++i ) {
	    *m_istr >> random_states[i];
	}
	*m_istr >> weights_size;
	WeightContainer weights(weights_size);
	for ( int ii = 0; ii < weights_size; ++ii ) *m_istr >> weights[ii];
	m_istr->ignore(2,'\n');
	// 
	// fill signal_process_id, event_number, weights, random_states
	evt->set_signal_process_id( signal_process_id );
	evt->set_event_number( event_number );
	evt->weights() = weights;
	evt->set_random_states( random_states );
	//
	// the end vertices of the particles are not connected until
	//  after the event is read --- we store the values in a map until then
       	std::map<GenParticle*,int> particle_to_end_vertex;
	//
	// read in the vertices
	for ( int iii = 1; iii <= num_vertices; ++iii ) {
          //std::cerr << "Reading V" << std::endl;
	    GenVertex* v = read_vertex(particle_to_end_vertex);
	    evt->add_vertex( v );
	}
	// set the signal process vertex
	if ( signal_process_vertex ) {
	    evt->set_signal_process_vertex( 
		evt->barcode_to_vertex(signal_process_vertex) );
	}
	//
	// last connect particles to their end vertices
	for ( std::map<GenParticle*,int>::iterator pmap 
		  = particle_to_end_vertex.begin(); 
	      pmap != particle_to_end_vertex.end(); ++pmap ) {
	    GenVertex* itsDecayVtx = evt->barcode_to_vertex(pmap->second);
	    if ( itsDecayVtx ) itsDecayVtx->add_particle_in( pmap->first );
	    else {
		std::cerr << "IO_Ascii_V12::fill_next_event ERROR particle points"
			  << "\n to null end vertex. " <<std::endl;
	    }
	}
	return 1;
    }

    void IO_Ascii_V12::write_comment( const std::string comment ) {
	// check the state of m_file is good, and that it is in output mode
	if ( !(*m_ostr) ) return;
	if ( (m_ostr == NULL) ) {
	    std::cerr << "HepMC::IO_Ascii_V12::write_particle_data_table "
		      << " attempt to write to input file." << std::endl;
	    return;
	}
	// write end of event listing key if events have already been written
	write_end_listing();
	// insert the comment key before the comment
	*m_ostr << "\n" << "HepMC::IO_Ascii-COMMENT\n";
	*m_ostr << comment << std::endl;
    }

    void IO_Ascii_V12::write_particle_data_table( const ParticleDataTable* pdt) {
	//
	// check the state of m_file is good, and that it is in output mode
	if ( !(*m_ostr) ) return;
	if ( (m_ostr == NULL) ) {
	    std::cerr << "HepMC::IO_Ascii_V12::write_particle_data_table "
		      << " attempt to write to input file." << std::endl;
	    return;
	}
	// write end of event listing key if events have already been written
	write_end_listing();
	//
	*m_ostr << "\n" << "HepMC::IO_Ascii-START_PARTICLE_DATA\n";
        for ( ParticleDataTable::const_iterator pd = pdt->begin(); 
	      pd != pdt->end(); pd++ ) {
	    write_particle_data( pd->second );
        }
	*m_ostr << "HepMC::IO_Ascii-END_PARTICLE_DATA\n" << std::flush;
    }

    bool IO_Ascii_V12::fill_particle_data_table( ParticleDataTable* pdt ) {
	//
	// test that pdt pointer is not null
	if ( !pdt ) {
	    std::cerr 
		<< "IO_Ascii_V12::fill_particle_data_table - passed null table." 
		<< std::endl;
	    return 0;
	}
	//
	// check the state of m_file is good, and that it is in input mode
	if ( !(*m_istr)) return 0;
	if ( m_istr == NULL ) {
	    std::cerr << "HepMC::IO_Ascii_V12::fill_particle_data_table "
		      << " attempt to read from output file." << std::endl;
	    return 0;
	}
	// position to beginning of file
	int initial_file_position = m_istr->tellg();
	std::ios::iostate initial_state = m_istr->rdstate();
        // gzipped reading may not work here...
	m_istr->seekg( 0 ); 
	// skip through the file just after first occurence of the start_key
	if (!search_for_key_end( *m_istr,
				 "HepMC::IO_Ascii-START_PARTICLE_DATA\n")) {
	    m_istr->seekg( initial_file_position );
	    std::cerr << "IO_Ascii_V12::fill_particle_data_table start key not  "
		      << "found setting badbit." << std::endl;
	    m_istr->clear(std::ios::badbit); 
	    return 0;
	}
	//
	pdt->set_description("Read with IO_Ascii_V12");
	// 
	// read Individual GenParticle data entries
	while ( read_particle_data( pdt ) );
	//
	// eat end_key
	if ( !eat_key(*m_istr,"HepMC::IO_Ascii-END_PARTICLE_DATA\n") ){
	    std::cerr << "IO_Ascii_V12::fill_particle_data_table end key not  "
		      << "found setting badbit." << std::endl;
	    m_istr->clear(std::ios::badbit);
	}
	// put the file back into its original state and position
	m_istr->clear( initial_state );
	m_istr->seekg( initial_file_position );
	return 1;
    }

    void IO_Ascii_V12::write_vertex( GenVertex* v ) {
	// assumes mode has already been checked
      if ( !v || !(*m_ostr) ) {
	    std::cerr << "IO_Ascii_V12::write_vertex !v||!(*m_ostr), "
		      << "v="<< v << " setting badbit" << std::endl;
	    m_ostr->clear(std::ios::badbit); 
	    return;
	}
	// First collect info we need
	// count the number of orphan particles going into v
	int num_orphans_in = 0;
	for ( GenVertex::particles_in_const_iterator p1
		  = v->particles_in_const_begin();
	      p1 != v->particles_in_const_end(); ++p1 ) {
	    if ( !(*p1)->production_vertex() ) ++num_orphans_in;
	}
	//
	*m_ostr << 'V';
	output( v->barcode() ); // v's unique identifier
	output( v->id() );
	output( v->position().x() );
	output( v->position().y() );
	output( v->position().z() );
	output( v->position().t() );
	output( num_orphans_in );
	output( (int)v->particles_out_size() );
	output( (int)v->weights().size() );
	for ( WeightContainer::iterator w = v->weights().begin(); 
	      w != v->weights().end(); ++w ) {
	    output( *w );
	}
	output('\n');
	for ( GenVertex::particles_in_const_iterator p2 
		  = v->particles_in_const_begin();
	      p2 != v->particles_in_const_end(); ++p2 ) {
	    if ( !(*p2)->production_vertex() ) {
		write_particle( *p2 );
	    }
	}
	for ( GenVertex::particles_out_const_iterator p3 
		  = v->particles_out_const_begin();
	      p3 != v->particles_out_const_end(); ++p3 ) {
	    write_particle( *p3 );
	}
    }

    void IO_Ascii_V12::write_particle( GenParticle* p ) {
	// assumes mode has already been checked
	if ( !p || !(*m_ostr) ) {
	    std::cerr << "IO_Ascii_V12::write_vertex !p||!(*m_ostr), "
		      << "v="<< p << " setting badbit" << std::endl;
	    m_ostr->clear(std::ios::badbit); 
	    return;
	}
	*m_ostr << 'P';
	output( p->barcode() );
	output( p->pdg_id() );
	output( p->momentum().px() );
	output( p->momentum().py() );
	output( p->momentum().pz() );
	output( p->momentum().e() );
	output( p->status() );
	output( p->polarization().theta() );
	output( p->polarization().phi() );
	// since end_vertex is oftentimes null, this CREATES a null vertex
	// in the map
	output(   ( p->end_vertex() ? p->end_vertex()->barcode() : 0 )  );
	*m_ostr << ' ' << p->flow() << "\n";
    }

    void IO_Ascii_V12::write_particle_data( const ParticleData* pdata ) {
	// assumes mode has already been checked
	if ( !pdata || !(*m_ostr) ) {
	    std::cerr << "IO_Ascii_V12::write_vertex !pdata||!(*m_ostr), "
		      << "pdata="<< pdata << " setting badbit" << std::endl;
	    m_ostr->clear(std::ios::badbit); 
	    return;
	}
	*m_ostr << 'D';
	output( pdata->pdg_id() );
	output( pdata->charge() );
	output( pdata->mass() );
	output( pdata->clifetime() );
	output( (int)(pdata->spin()*2.+.1) );
	// writes the first 21 characters starting with 0
	*m_ostr << " " << pdata->name().substr(0,21) << "\n";
    }

    GenVertex* IO_Ascii_V12::read_vertex
    ( std::map<GenParticle*,int>& particle_to_end_vertex )
    {
	// assumes mode has already been checked
	//
	// test to be sure the next entry is of type "V" then ignore it
	if ( !(*m_istr) || m_istr->peek()!='V' ) {
	    std::cerr << "IO_Ascii_V12::read_vertex setting badbit." << std::endl;
	    m_istr->clear(std::ios::badbit); 
	    return 0;
	} 
	m_istr->ignore();
	// read values into temp variables, then create a new GenVertex object
	int identifier =0, id =0, num_orphans_in =0, 
            num_particles_out = 0, weights_size = 0;
	double x = 0., y = 0., z = 0., t = 0.; 
	*m_istr >> identifier >> id >> x >> y >> z >> t
	       >> num_orphans_in >> num_particles_out >> weights_size;
	WeightContainer weights(weights_size);
	for ( int i1 = 0; i1 < weights_size; ++i1 ) *m_istr >> weights[i1];
	m_istr->ignore(2,'\n');
	GenVertex* v = new GenVertex( FourVector(x,y,z,t),
				id, weights);
	v->suggest_barcode( identifier );
	//
	// read and create the associated particles. outgoing particles are
	//  added to their production vertices immediately, while incoming
	//  particles are added to a map and handles later.
	for ( int i2 = 1; i2 <= num_orphans_in; ++i2 ) {
	    read_particle(particle_to_end_vertex);
	}
	for ( int i3 = 1; i3 <= num_particles_out; ++i3 ) {
	    v->add_particle_out( read_particle(particle_to_end_vertex) );
	}
	return v;
    }

    GenParticle* IO_Ascii_V12::read_particle(
	std::map<GenParticle*,int>& particle_to_end_vertex ){
	// assumes mode has already been checked
	//
	// test to be sure the next entry is of type "P" then ignore it
        //std::cerr << "Reading P: " << m_istr->peek() << std::endl;
	if ( !(*m_istr) || m_istr->peek()!='P' ) { 
	    std::cerr << "IO_Ascii_V12::read_particle setting badbit." 
		      << std::endl;
	    m_istr->clear(std::ios::badbit); 
	    return 0;
	} 
	m_istr->ignore();
	//
	// declare variables to be read in to, and read everything except flow
	double px = 0., py = 0., pz = 0., e = 0., theta = 0., phi = 0.;
	int bar_code = 0, id = 0, status = 0, end_vtx_code = 0, flow_size = 0;
        if (m_format_version == 2) {
          *m_istr >> bar_code >> id >> px >> py >> pz >> e >> status 
                  >> theta >> phi >> end_vtx_code >> flow_size;
        } else if (m_format_version == 1) {
          double mass;
          *m_istr >> bar_code >> id >> px >> py >> pz >> e >> mass >> status 
                  >> theta >> phi >> end_vtx_code >> flow_size;
        } else {
          std::cerr << "IO_Ascii_V12::read_particle encountered internal error "
                    << "(unrecognised format version)" << std::endl;
            return 0;
        }
	//
	// read flow patterns if any exist
	Flow flow;
	int code_index, code;
	for ( int i = 1; i <= flow_size; ++i ) {
	    *m_istr >> code_index >> code;
	    flow.set_icode( code_index,code);
	}
	m_istr->ignore(2,'\n'); // '\n' at end of entry
	GenParticle* p = new GenParticle( FourVector(px,py,pz,e), 
				    id, status, flow, 
				    Polarization(theta,phi) );
	p->suggest_barcode( bar_code );
	//
	// all particles are connected to their end vertex separately 
	// after all particles and vertices have been created - so we keep
	// a map of all particles that have end vertices
	if ( end_vtx_code != 0 ) particle_to_end_vertex[p] = end_vtx_code;
	return p;
    }

    ParticleData* IO_Ascii_V12::read_particle_data( ParticleDataTable* pdt ) {
	// assumes mode has already been checked
	//
	// test to be sure the next entry is of type "D" then ignore it
	if ( !(*m_istr) || m_istr->peek()!='D' ) return 0;
	m_istr->ignore();
	//
	// read values into temp variables then create new ParticleData object
	char its_name[22];
	int its_id = 0, its_spin = 0;  
	double its_charge = 0, its_mass = 0, its_clifetime = 0;
	*m_istr >> its_id >> its_charge >> its_mass 
	       >> its_clifetime >> its_spin;
	m_istr->ignore(1); // eat the " "
	m_istr->getline( its_name, 22, '\n' );
	ParticleData* pdata = new ParticleData( its_name, its_id, its_charge, 
						its_mass, its_clifetime, 
						double(its_spin)/2.);
	pdt->insert(pdata);
	return pdata;
    }

    bool IO_Ascii_V12::write_end_listing() {
    	if ( m_finished_first_event_io && (m_ostr != NULL) ) {
	    *m_ostr << "HepMC::IO_Ascii-END_EVENT_LISTING\n" << std::flush;
	    m_finished_first_event_io = 0;
	    return 1;
	}
	return 0;
    }

    bool IO_Ascii_V12::search_for_key_end( std::istream& in, const char* key ) {
	/// reads characters from in until the string of characters matching
	/// key is found (success) or EOF is reached (failure).
	/// It stops immediately thereafter. Returns T/F for success/fail
	// 
	char c[1];
	unsigned int index = 0;
	while ( in.get(c[0]) ) {
	    if ( c[0] == key[index] ) { 
		++index;
	    } else { index = 0; }
	    if ( index == strlen(key) ) return 1;
	}
	return 0;
    }

    int IO_Ascii_V12::search_for_2key_end( std::istream& in, 
                                           const char* key1,
                                           const char* key2) {
      /// reads characters from in until the string of characters matching
      /// key1 or key2 is found (returns 1 or 2) or EOF is reached (returns 0).
      /// It stops immediately thereafter.
      ///
      /// Currently it assumes that key1 and key2 start with different letters
      assert(key1[0] != key2[0]);
      char c[1];
      unsigned int index = 0;
      int ikey = 0;
      while ( in.get(c[0]) ) {
        if (index == 0) {
          if        (c[0] == key1[index]) {
            ikey = 1;
            ++index;
          } else if (c[0] == key2[index]) {
            ikey = 2;
            ++index;
          }
        } else {
          if        (ikey == 1 && c[0] == key1[index]) {
            index++;
            if ( index == strlen(key1) ) return 1;
          } else if (ikey == 2 && c[0] == key2[index]) {
            index++;
            if ( index == strlen(key2) ) return 2;
          } else {
            index = 0;
          }
        }
      }
      return 0;
    }

    bool IO_Ascii_V12::search_for_key_beginning( std::istream& in,
					     const char* key ) {
	/// not tested and NOT used anywhere!
	if ( search_for_key_end( in, key) ) {
	    int i = strlen(key);
	    while ( i>=0 ) in.putback(key[i--]); 
            return 1; 
	} else {
	    in.putback(EOF);
	    in.clear();
            return 0;
	}
    }

    bool IO_Ascii_V12::eat_key( std::istream& in, const char* key ) {
	/// eats the character string key from istream in - only if the key
	/// is the very next occurence in the stream
	/// if the key is not the next occurence, it eats nothing ... i.e.
	///  it puts back whatever it would have eaten.
	int key_length = strlen(key);
	// below is the only way I know of to get a variable length string
	//  conforming to ansi standard.
	char* c = new char[key_length +1];
	int i=0;
	// read the stream until get fails (because of EOF), a character
	//  doesn't match a character in the string, or all characters in
	//  the string have been checked and match.
	while ( in.get(c[i]) && c[i]==key[i] && i<key_length ) {
	    ++i;
	}
	if ( i == key_length ) {
	    delete c;
	    return 1;
	}
	//
	// if we get here, then we have eaten the wrong this and we must put it
	// back
	while ( i>=0 ) in.putback(c[i--]); 
	delete c;
	return 0;
    }

    int IO_Ascii_V12::find_in_map( const std::map<GenVertex*,int>& m, 
			       GenVertex* v ) const {
	std::map<GenVertex*,int>::const_iterator iter = m.find(v);
	if ( iter == m.end() ) return 0;
	return iter->second;
    }
	
} // HepMC






