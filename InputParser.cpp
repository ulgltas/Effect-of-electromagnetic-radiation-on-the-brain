#include "InputParser.h"

#include <algorithm>
#include <cctype>

#include <cstring>

#include <cmath>


// [RB] cette classe semble inutilement complexe

// Small enums for the dollar strings (see InputParser::readHeader)
stringDollar_Header1 InputParser::hashit_Header1 (std::string const& inString) {
    if (inString == "INFOS") return INFOS;
    if (inString == "MESH") return MESH;
    if (inString == "RUN_INFOS") return RUN_INFOS;
	else {
		printf("In file %s at %d. Complain to Romin. Abort().\n",__FILE__,__LINE__);
		abort();
	}
}
stringDollar_Header2 InputParser::hashit_Header2 (std::string const& inString) {
    if (inString == "NAME") return NAME;
    if (inString == "DELTAS") return DELTAS;
    if (inString == "DOMAIN_SIZE") return DOMAIN_SIZE;
	if (inString == "SOURCE") return SOURCE;
	if (inString == "STOP_SIMUL_AFTER") return STOP_SIMUL_AFTER;
	if (inString == "TEMP_INIT") return TEMP_INIT;
	if (inString == "MATERIALS") return MATERIALS;
	if (inString == "ORIGINS")   return ORIGINS;
	else {
		printf("In file %s at %d. Complain to Romin. Abort().\n",__FILE__,__LINE__);
		cout << "Faulty string is ::" + inString + "::" << endl;
		abort();
	}
}

// Get lengths
double InputParser::get_length_WholeDomain(unsigned int direction,std::string type){
	if(strcmp(type.c_str(),"ELECTRO") == 0){
		// Direction = 0 gives along X, 1 along Y, 2 along Z:
		if(direction == 0){
			return this->lengthX_WholeDomain_Electro;
		}else if(direction == 1){
			return this->lengthY_WholeDomain_Electro;
		}else if(direction == 2){
			return this->lengthZ_WholeDomain_Electro;
		}else{
			printf("InputParser::get_length::ERROR:\n\tDirection should be between 0 and 2.");
			printf("Aborting (file %s at %d).\n",__FILE__,__LINE__);
			abort();
		}
	}else if(strcmp(type.c_str(),"THERMAL") == 0){
			// Direction = 0 gives along X, 1 along Y, 2 along Z:
		if(direction == 0){
			return this->lengthX_WholeDomain_Thermal;
		}else if(direction == 1){
			return this->lengthY_WholeDomain_Thermal;
		}else if(direction == 2){
			return this->lengthZ_WholeDomain_Thermal;
		}else{
			printf("InputParser::get_length::ERROR:\n\tDirection should be between 0 and 2.");
			printf("Aborting (file %s at %d).\n",__FILE__,__LINE__);
			abort();
		}
	}else{
		fprintf(stderr,"InputParser::get_length_WholeDomain::ERROR\n");
		fprintf(stderr,"File %s:%d\n",__FILE__,__LINE__);
		abort();
	}
}

InputParser::InputParser(string file_name){
		#if DEBUG > 2
		cout << "InputParser::constructor::IN\n";
		#endif
		this->filename = file_name;
		#if DEBUG > 2
		cout << "InputParser::constructor::OUt\n"; // [RB] ne sert a rien - aucune chance que l'assignation ci-dessus plante
		#endif
}

void InputParser::defaultParsingFromFile(void){
	if(this->filename == string()){
		cout << "\nInputParser::defaultParsingFromFile\n\tNo file provided. ABORTING.\n\n";
		abort();
	}else{
		// Check that the file exist:
		if(this->is_file_exist(this->filename)){
			#if DEBUG > 1
			cout << "Input file exist !" << endl;
			#endif
		}else{
			cout << "InputParser::defaultParsingFromFile\n";
			cout << "Input file doesn't exist ! Aborting.\n";
			abort();
		}
		// Go to the parsing function:
		this->basicParsing(this->filename);
	}
}

void InputParser::defaultParsingFromFile(string filename){
	// Check that the file exists:
	if(this->is_file_exist(filename)){
		#if DEBUG > 1
		cout << "Input file exist !" << endl;
		#endif
	}else{
		cout << "InputParser::defaultParsingFromFile\n";
		cout << "Input file doesn't exist ! Aborting.\n";
		abort();
	}
	// Go to the parsing function:
	this->basicParsing(filename);
}

bool InputParser::is_file_exist(const string fileName){
	cout << fileName << endl;
	#if DEBUG > 4
	cout << "InputParser::is_file_exist::IN\n";
	#endif
    ifstream infile(fileName);
	#if DEBUG > 4
	cout << "InputParser::is_file_exist::OUT\n";
	#endif
    return infile.good();
}

void InputParser::basicParsing(const string filename){
	// Check the extension of the file:
	if(filename.substr(filename.find_last_of(".")+1) == "input"){
		// The extension is correct, proceed.
		// Open the file for reading:
		ifstream inputFile;
		inputFile.open(filename);
		if(inputFile.fail()){
			// Opening failed, aborting.
			cout << "InputParser::basicParsing::Failed to open ";
			cout << filename + ". Aborting.\n";
			inputFile.clear();
			abort();
		}else if(inputFile.is_open()){
			// Contains the current read line of the input file:
			string currentLine;
			
			// Looping on the whole file:
			while(!inputFile.eof()){

				// Get line:
				getline(inputFile,currentLine);
				
				// Check that the line is not a comment:
				this->checkLineISNotComment(inputFile,currentLine);

				if(currentLine.find("$") != std::string::npos){
					// If there is a dollar, we begin a section:
					this->readHeader(inputFile,currentLine);
				}
			}
		}else{ // [RB] ??????
			cout << "InputParser::basicParsing::Should not end up here !";
			cout << " Complain to the developer.\n";
			cout << "Aborting.\n";
			abort();
		}
		inputFile.close();
	}else{
		cout << "The input file is not under .input format. Please check your input file";
		cout << " " + filename << endl;
		cout << "Aborting.\n";
		std::abort();
	}
}

void InputParser::readHeader(ifstream &file,std::string &currentLine){
	// We are in a Dollar zone.
	// First, detect which dollar zone it is.
	// Get the string after the dollar:
	std::string strHeader1 = currentLine.substr(currentLine.find("$")+1);
	// Remove any space before the switch:
	strHeader1.erase(std::remove_if(strHeader1.begin(),
			 strHeader1.end(), [](unsigned char x){return std::isspace(x);}),
			 strHeader1.end());
	
	// Go with the switch:
	switch(hashit_Header1(strHeader1)){

		case INFOS    : 
			this->readHeader_INFOS(file);
			break;

		case MESH     : 
			this->readHeader_MESH (file);
			break;

		case RUN_INFOS: 
			this->readHeader_RUN_INFOS(file);
			break;

		default:
			printf("Should not end up here. Complain to Romin. Abort.");
			printf("(in file %s at %d)\n",__FILE__,__LINE__);
			abort();
	}
}

// Check that the line is not a comment:
bool InputParser::checkLineISNotComment(ifstream &file, string &currentLine){
	/* Check for line(s) being comments or blank */
	this->RemoveAnyBlankSpaceInStr(currentLine);
	while(currentLine == string() && !file.eof()){
		#if DEBUG > 4
		cout << "It is a blank line !" << endl;
		#endif
		getline(file,currentLine);
		this->RemoveAnyBlankSpaceInStr(currentLine);
	}

	const std::string comment1_beg = "/*";
	const std::string comment1_end = "*/";
	const std::string comment2 = "//";

	if(currentLine.find(comment1_beg) != std::string::npos){
		// We have multiple lines comment ! Read all the comment before exiting.
		while(!file.eof()){
			if(currentLine.find(comment1_end) != std::string::npos){
				getline(file,currentLine);
				printf("Stuck in multiple !\n");
				this->checkLineISNotComment(file,currentLine);
				break;
			}
			getline(file,currentLine);
		}
		return true;
	}else if(currentLine.find(comment2) != std::string::npos){
		// We have one line comment !
		while(!file.eof()){
			if(currentLine.find(comment2) == std::string::npos){
				printf("Stuck in single ! \n");
				this->checkLineISNotComment(file,currentLine);
				break;
			}
			getline(file,currentLine);
		}
	}else{
		return false;
	}
	return false;	
}

void InputParser::RemoveAnyBlankSpaceInStr(std::string &str){
	str.erase(std::remove_if(str.begin(),
			 str.end(), [](unsigned char x){return std::isspace(x);}),
			 str.end());
}

void InputParser::readHeader_INFOS(ifstream &file){
	//Inside the header(1) called 'INFOS', we have fields:
	//		1) NAME - Contains fields:
	//				a) output : name of the output files
	//				b) error  : name of the error file
	//				c) profile: name of the profiling file (cpu time, etc)
	std::string currentLine = string();
	
	while(currentLine != "INFOS"){
		// Read line:
		getline(file,currentLine);
		// Get rid of comments:
		this->checkLineISNotComment(file,currentLine);
		// Remove any blank space:
		this->RemoveAnyBlankSpaceInStr(currentLine);
		// Remove Dollar sign:
		currentLine = currentLine.substr(currentLine.find("$")+1);
		cout << "BEFORE THE SWITCH : " + currentLine << endl;
		if(currentLine == "INFOS"){break;}
		switch(this->hashit_Header2(currentLine)){
			case NAME:
				cout << "Entering case name\n";
				while(!file.eof()){
					cout << "Entering while\n";
					// Note: sections are ended by $the-section-name.
					getline(file,currentLine);
					this->checkLineISNotComment(file,currentLine);
					this->RemoveAnyBlankSpaceInStr(currentLine);
					cout << currentLine << endl;
					if(currentLine == "$NAME"){
						cout << "EXITING NAME\n";
						break;
					}
					if(currentLine == string()){continue;}
					std::size_t posEqual  = currentLine.find("=");
					std::string propName  = currentLine.substr(0,posEqual); 
					std::string propGiven = currentLine.substr(posEqual+1,currentLine.length());
					cout << propName + "=" + propGiven << endl;
					cout << "To compare with " + currentLine << endl;
					if(propName != "output" && propName != "error" && propName != "profile"){
						printf("InputParser::readHeader_INFOS:: You didn't provide a ");
						printf("good member for $INFOS$NAME.\nAborting.\n");
						cout << propName << endl;
						printf("(in file %s at %d)\n",__FILE__,__LINE__);
						abort();
					}
					this->outputNames[propName] = propGiven;
				}
				break;
			default:
				printf("Should not end up here. Complain to Romin. Abort.");
				printf("(in file %s at %d)\n",__FILE__,__LINE__);
				cout << "Faulty line is " + currentLine << endl << endl;
				abort();
		}
	}
}

void InputParser::readHeader_MESH (ifstream &file){
	/* Inside the section MESH, thee are fields:
	 *		1) DELTAS containing:
	 * 				a) deltaX
	 * 				b) deltaY
	 *				c) deltaZ
	 */
	std::string currentLine = string();
	while(currentLine != "MESH"){
		// Read line:
		getline(file,currentLine);
		// Get rid of comments:
		this->checkLineISNotComment(file,currentLine);
		// Remove any blank space:
		this->RemoveAnyBlankSpaceInStr(currentLine);
		// Remove Dollar sign:
		currentLine = currentLine.substr(currentLine.find("$")+1);
		cout << "BEFORE THE SWITCH : " + currentLine << endl;
		if(currentLine == "MESH"){break;}
		switch(this->hashit_Header2(currentLine)){
			case DELTAS:
				cout << "Entering case DELTAS\n";
				while(!file.eof()){
					// Note: sections are ended by $the-section-name.
					// Read line:
					getline(file,currentLine);
					// Get rid of comments:
					this->checkLineISNotComment(file,currentLine);
					// Remove any blank in the string:
					this->RemoveAnyBlankSpaceInStr(currentLine);
					cout << currentLine << endl;
					// If the string is "$DELTAS" it means the section ends.
					if(currentLine == "$DELTAS"){
						cout << "EXITING DELTAS\n";
						cout << "Veryfying compatibility between EM and TH grids...\n";
						// Check all electromagnetic deltas are equal !
						if(this->deltaX_Electro == this->deltaY_Electro
							&& this->deltaX_Electro == this->deltaZ_Electro
							&& this->deltaY_Electro == this->deltaZ_Electro)
						{
							// Check the ratio:
							double ratio = this->deltaX_Electro / this->delta_Thermal;

							// Use absolute value to keep rounding error away.

							if(abs(ratio - this->ratio_EM_TH_delta) > ratio*1E-8){
								fprintf(stderr,"InputParser::Wrong spatial steps\n");
								fprintf(stderr,"Spatial step EM is %f.",this->deltaX_Electro);
								fprintf(stderr,"Spatial step TH is %f.",this->delta_Thermal);
								fprintf(stderr,"Announced ratio is %f but has %f.\n",
									this->ratio_EM_TH_delta,ratio);
								fprintf(stderr,"Aborting.\nFile %s:%d\n",__FILE__,__LINE__);
								std::abort();
							}
						}else{
							fprintf(stderr,"InputParser::wrong electromagnetic time steps.\n");
							fprintf(stderr,"Spatial steps for EM grid should be equal.\nAborting.\n");
							fprintf(stderr,"In %s:%d\n",__FILE__,__LINE__);
							std::abort();
						}
						break;
					}
					// If the string is empty, it was just a white space. Continue.
					if(currentLine == string()){continue;}
					// Find the position of the equal sign:
					std::size_t posEqual  = currentLine.find("=");
					// The property we want to set:
					std::string propName  = currentLine.substr(0,posEqual);
					// The property name the user gave:
					std::string propGiven = currentLine.substr(posEqual+1,currentLine.length());

					cout << propName + "=" + propGiven << endl;
					cout << "To compare with " + currentLine << endl;

					// Spatial step delta along X for electromagnetic mesh:
					if(propName == "deltaX_Electro"){
						this->deltaX_Electro = std::stod(propGiven);
						cout << "ADDED DELTAX_Electro is " << this->deltaX_Electro << std::endl;
					// Spatial step delta along Y for electromagnetic mesh:
					}else if(propName == "deltaY_Electro"){
						this->deltaY_Electro = std::stod(propGiven);
						cout << "ADDED DELTAY_Electro is " << this->deltaY_Electro << std::endl;
					// Spatial step delta along Z for electromagnetic mesh:
					}else if(propName == "deltaZ_Electro"){
						this->deltaZ_Electro = std::stod(propGiven);
						cout << "ADDED DELTAZ is " << this->deltaZ_Electro << std::endl;
					// Spatial step delta along all three directions for the thermal mesh:
					}else if(propName == "delta_Thermal"){
						this->delta_Thermal = std::stod(propGiven);
						cout << "ADDED DELTA THERMAL is " << this->delta_Thermal << std::endl;
					// Ratio between the spatial and thermal grids
					// If deltaElectro = 4 and deltaThermal = 2, the ratio is 0.5.
					}else if(propName == "ratio_EM_TH_delta"){
						this->ratio_EM_TH_delta = std::stod(propGiven);
						cout << "ADDED ratio_EM_TH_delta is " << this->ratio_EM_TH_delta << std::endl;

					}else if(propName != "deltaX_electro" 
							&& propName != "deltaY_Electro" 
							&& propName != "deltaZ_Electro"
							&& propName != "delta_Thermal"){
						printf("InputParser::readHeader_MESH:: You didn't provide a ");
						printf("good member for $MESH$DELTAS. Has %s.\nAborting.\n",propName.c_str());
						cout << propName << endl;
						printf("(in file %s at %d)\n",__FILE__,__LINE__);
						abort();
					}
				}
				break;
			
			
			case DOMAIN_SIZE:
				cout << "Entering case DOMAIN_SIZE\n";
				while(!file.eof()){
					// Note: sections are ended by $the-section-name.
					// Read line:
					getline(file,currentLine);
					// Get rid of comments:
					this->checkLineISNotComment(file,currentLine);
					// Remove any blank in the string:
					this->RemoveAnyBlankSpaceInStr(currentLine);
					cout << currentLine << endl;
					// If the string is "$DELTAS" it means the section ends.
					if(currentLine == "$DOMAIN_SIZE"){
						cout << "EXITING DOMAIN_SIZE\n";
						break;
					}
					// If the string is empty, it was just a white space. Continue.
					if(currentLine == string()){continue;}
					// Find the position of the equal sign:
					std::size_t posEqual  = currentLine.find("=");
					// The property we want to set:
					std::string propName  = currentLine.substr(0,posEqual);
					// The property name the user gave:
					std::string propGiven = currentLine.substr(posEqual+1,currentLine.length());

					cout << propName + "=" + propGiven << endl;
					cout << "To compare with " + currentLine << endl;

					if(propName == "L_X_ELECTRO"){
						this->lengthX_WholeDomain_Electro = std::stod(propGiven);
						cout << "ADDED lengthX_WholeDomain_Electro is " << this->lengthX_WholeDomain_Electro << endl;

					}else if(propName == "L_Y_ELECTRO"){
						this->lengthY_WholeDomain_Electro = std::stod(propGiven);
						cout << "ADDED lengthY_WholeDomain_Electro is " << this->lengthY_WholeDomain_Electro << endl;

					}else if(propName == "L_Z_ELECTRO"){
						this->lengthZ_WholeDomain_Electro = std::stod(propGiven);
						cout << "ADDED lengthZ_WholeDomain_Electro is " << this->lengthZ_WholeDomain_Electro << endl;

					}else if(propName == "L_X_THERMAL"){
						this->lengthX_WholeDomain_Thermal = std::stod(propGiven);
						cout << "ADDED lengthX_WholeDomain_Thermal is " << this->lengthX_WholeDomain_Thermal << endl;

					}else if(propName == "L_Y_THERMAL"){
						this->lengthY_WholeDomain_Thermal = std::stod(propGiven);
						cout << "ADDED lengthY_WholeDomain_Thermal is " << this->lengthY_WholeDomain_Thermal << endl;

					}else if(propName == "L_Z_THERMAL"){
						this->lengthZ_WholeDomain_Thermal = std::stod(propGiven);
						cout << "ADDED lengthZ_WholeDomain_Thermal is " << this->lengthZ_WholeDomain_Thermal << endl;
					
					}else if(propName != "L_X_ELECTRO" 
							&& propName != "L_Y_ELECTRO" 
							&& propName != "L_Z_ELECTRO"
							&& propName != "L_X_THERMAL"
							&& propName != "L_Y_THERMAL"
							&& propName != "L_Z_THERMAL"){
						printf("InputParser::readHeader_MESH:: You didn't provide a ");
						printf("good member for $MESH$DOMAIN_SIZE.\nAborting.\n");
						cout << propName << endl;
						printf("(in file %s at %d)\n",__FILE__,__LINE__);
						abort();
					}
				}
				break;

			
			case SOURCE:
				{
					bool nbr_Sources_Defined = false;
					std::cout << "Entering case SOURCE\n";
					while(!file.eof()){
						// Note: sections are ended by $the-section-name.
						// Read line:
						getline(file,currentLine);
						// Get rid of comments:
						this->checkLineISNotComment(file,currentLine);
						// Remove any blank in the string:
						this->RemoveAnyBlankSpaceInStr(currentLine);
						// If the string is "$DELTAS" it means the section ends.
						if(currentLine == "$SOURCE"){
							std::cout << "EXITING SOURCE\n";
							break;
						}
						// If the string is empty, it was just a white space. Continue.
						if(currentLine == string()){continue;}
						// Find the position of the equal sign:
						std::size_t posEqual  = currentLine.find("=");
						// The property we want to set:
						std::string propName  = currentLine.substr(0,posEqual);
						// The property name the user gave:
						std::string propGiven = currentLine.substr(posEqual+1,currentLine.length());

						std::cout << propName + "=" + propGiven << endl;
						std::cout << "To compare with " + currentLine << endl;

						if(propName != "NBR_SOURCES" && nbr_Sources_Defined == false){
							printf("InputParser::readHeader_MESH::CASE SOURCE\n");
							printf("You must first set the number of sources. Aborting.\n");
							std::abort();
						}

						if(propName == "NBR_SOURCES"){
							this->source.set_number_of_sources(std::stod(propGiven));
							std::cout << "NBR_SOURCES set to ";
							std::cout << this->source.get_number_of_sources() << endl;
							nbr_Sources_Defined = true;

						}else if(propName == "L_X"){
							std::vector<double> temp = 
								this->determineVectorFromStr(
									propGiven,
									this->source.get_number_of_sources());
							cout << "Values are : " << temp[0] << temp[1] << endl;
							this->source.setLengthAlongOneDir(0,temp);

						}else if(propName == "L_Y"){
							std::vector<double> temp = 
								this->determineVectorFromStr(
									propGiven,
									this->source.get_number_of_sources());
							cout << temp[0] << temp[1] << endl;
							this->source.setLengthAlongOneDir(1,temp);

						}else if(propName == "L_Z"){
							std::vector<double> temp = 
								this->determineVectorFromStr(
									propGiven,
									this->source.get_number_of_sources());
							cout << temp[0] << temp[1] << endl;
							this->source.setLengthAlongOneDir(2,temp);

						}else if(propName == "C_X"){
							std::vector<double> temp = 
								this->determineVectorFromStr(
									propGiven,
									this->source.get_number_of_sources());
							cout << temp[0] << temp[1] << endl;
							this->source.setCenterAlongOneDir(0,temp);

						}else if(propName == "C_Y"){
							std::vector<double> temp = 
								this->determineVectorFromStr(
									propGiven,
									this->source.get_number_of_sources());
							cout << temp[0] << temp[1] << endl;
							this->source.setCenterAlongOneDir(1,temp);

						}else if(propName == "C_Z"){
							std::vector<double> temp = 
								this->determineVectorFromStr(
									propGiven,
									this->source.get_number_of_sources());
							cout << temp[0] << temp[1] << endl;
							this->source.setCenterAlongOneDir(2,temp);

						}else if(propName == "FRQCY"){
							std::vector<double> temp = 
								this->determineVectorFromStr(
									propGiven,
									this->source.get_number_of_sources());
							cout << temp[0] << temp[1] << endl;
							this->source.setAllFrequencies(temp);

						}else if(propName == "AIR_GAP"){
							std::vector<double> airgaps = 
								this->determineVectorFromStr(
									propGiven,
									this->source.get_number_of_sources());
							cout << airgaps[0] << airgaps[1] << endl;
							cout << "Setting AriGaps...";
							this->source.set_airGaps(airgaps);
							cout << "Done.\n";
						
						}else if(propName != "NBR_SOURCES" 
								&& propName != "L_X" 
								&& propName != "L_Y"
								&& propName != "L_Z"){
							printf("InputParser::readHeader_MESH:: You didn't provide a ");
							printf("good member for $MESH$SOURCE.\nAborting.\n");
							cout << propName << endl;
							printf("(in file %s at %d)\n",__FILE__,__LINE__);
							abort();
						}
					}
				}
				break;
			case MATERIALS:
				cout << "Entering case MATERIALS\n";
				while(!file.eof()){
					// Note: sections are ended by $the-section-name.
					// Read line:
					getline(file,currentLine);
					// Get rid of comments:
					this->checkLineISNotComment(file,currentLine);
					// Remove any blank in the string:
					this->RemoveAnyBlankSpaceInStr(currentLine);
					cout << currentLine << endl;
					// If the string is "$DELTAS" it means the section ends.
					if(currentLine == "$MATERIALS"){
						cout << "EXITING MATERIALS\n";
						break;
					}
					// If the string is empty, it was just a white space. Continue.
					if(currentLine == string()){continue;}
					// Find the position of the equal sign:
					std::size_t posEqual  = currentLine.find("=");
					// The property we want to set:
					std::string propName  = currentLine.substr(0,posEqual);
					// The property name the user gave:
					std::string propGiven = currentLine.substr(posEqual+1,currentLine.length());

					cout << propName + "=" + propGiven << endl;
					cout << "To compare with " + currentLine << endl;

					if(propName == "USE_AIR_EVERYWHERE"){
						if(propGiven == "true"){
							this->simulationType = "USE_AIR_EVERYWHERE";
							cout << "SET simulationType to USE_AIR_EVERYWHERE\n";
						}else{
							printf("You set USE_AIR_EVERYWHERE");
							printf(" to false.Aborting.\n");
							std::abort();
						}

					}else if(propName == "TEST_PARAVIEW"){
						if(propGiven == "true"){
							this->simulationType = "TEST_PARAVIEW";
							std::cout << "SET simulationType to TEST_PARAVIEW" << std::endl;
						}else{
							printf("You set TEST_PARAVIEW to false. Aborting.\n");
							std::abort();
						}

					}else if(propName == "TEST_PARAVIEW_MPI"){
						// Assign the simulation type:
						this->simulationType = "TEST_PARAVIEW_MPI";
						std::cout << "SET simulationType to TEST_PARAVIEW_MPI" << std::endl;
						// Parse the propGiven:
						/**
						 * The 'propGiven' must be (A,B,C), i.e. comma-separated
						 * and surrounded with brackets.
						 */
						/// Remove the brackets:
						if(propGiven.c_str()[0] == '('){
							printf("Have parenthesis: ok\n");
							/// Check the last character is a bracket:
							if(propGiven.c_str()[propGiven.size()-1] == ')'){
								printf("Have closing parenthesis ok\n");
								/// Erase the two parenthesis from the 'propGiven' string:
								propGiven = propGiven.substr(1, propGiven.size() -2);
							}else{
								fprintf(stderr,"In %s :: while parsing the arguments of 'TEST_PARAVIEW_MPI.\n",
									__FUNCTION__);
								fprintf(stderr,"You miss the closing parenthesis !\n");
								fprintf(stderr,"File %s:%d\n",__FILE__,__LINE__);
								#ifdef MPI_COMM_WORLD
								MPI_Abort(MPI_COMM_WORLD,-1);
								#else
								abort();
								#endif
							}
						}else{
							fprintf(stderr,"In %s :: while parsing the arguments of 'TEST_PARAVIEW_MPI.\n",
									__FUNCTION__);
								fprintf(stderr,"You miss the opening parenthesis !\n");
								fprintf(stderr,"File %s:%d\n",__FILE__,__LINE__);
								#ifdef MPI_COMM_WORLD
								MPI_Abort(MPI_COMM_WORLD,-1);
								#else
								abort();
								#endif
						}
						std::vector<std::string> args;
						std::stringstream ss(propGiven);
						while(ss.good()){
							std::string substr;
							getline(ss,substr,',');
							args.push_back(substr);
							std::cout << substr << std::endl;
						}
						/// If there is more than 3 args, abort:
						if(args.size() > 3){
							fprintf(stderr,"In %s :: while parsing the arguments of 'TEST_PARAVIEW_MPI.\n",
									__FUNCTION__);
							fprintf(stderr,"You must given 3 args: TEMP=sthg,E=sthgElse,H=sthg !\n");
							fprintf(stderr,"File %s:%d\n",__FILE__,__LINE__);
							#ifdef MPI_COMM_WORLD
							MPI_Abort(MPI_COMM_WORLD,-1);
							#else
							abort();
							#endif
						}
						std::vector<std::string> ARGS_ORDER = {"TEMP","E","H"};
						for(size_t i = 0 ; i < 3 ; i ++ ){
							if(args[i].find(ARGS_ORDER[i]) == std::string::npos){
								/// Cannot find the argument TEMP that should come first. Aborting.
								fprintf(stderr,"In %s :: while parsing the arguments of 'TEST_PARAVIEW_MPI.\n",
										__FUNCTION__);
								fprintf(stderr,"You must provide %s !\n",ARGS_ORDER[i].c_str());
								fprintf(stderr,"File %s:%d\n",__FILE__,__LINE__);
								#ifdef MPI_COMM_WORLD
								MPI_Abort(MPI_COMM_WORLD,-1);
								#else
								abort();
								#endif
							}else{
								std::size_t posEqual   = args[i].find("=");
								std::string prop_name  = args[i].substr(0,posEqual);
								if(prop_name != ARGS_ORDER[i]){
									fprintf(stderr,"FATAL ERROR\n");
									fprintf(stderr,"FILE %s:%d\n",__FILE__,__LINE__);
									abort();
								}
								std::string prop_given = args[i].substr(posEqual+1,args[i].length());
								printf("%s has been set to %s.\n",prop_name.c_str(),prop_given.c_str());
								this->TEST_PARAVIEW_MPI_ARGS.insert(
									std::pair<std::string,std::string>(
										prop_name,
										prop_given
									));
							}
						}

						/* END OF TEST_PARAVIEW_MPI */
						

					}else{
						printf("InputParser::readHeader_MESH:: You didn't provide a ");
						printf("good member for $MESH$MATERIALS.\nAborting.\n");
						cout << propName << endl;
						printf("(in file %s at %d)\n",__FILE__,__LINE__);
						abort();
					}
				}
				/* Check that simulation type was effectively set: */
				if(this->simulationType.get_alreadySet() == false){
					printf("InputParser::readHeader_MESH::MATERIALS\n");
					printf("Exiting section MATERIALS without specifying");
					printf(" anything. Aborting().\n\n");
					abort();
				}
				break;

			case ORIGINS:
				cout << "Entering case ORIGINS\n";
				while(!file.eof()){
					// Note: sections are ended by $the-section-name.
					// Read line:
					getline(file,currentLine);
					// Get rid of comments:
					this->checkLineISNotComment(file,currentLine);
					// Remove any blank in the string:
					this->RemoveAnyBlankSpaceInStr(currentLine);
					cout << currentLine << endl;
					// If the string is "$DELTAS" it means the section ends.
					if(currentLine == "$ORIGINS"){
						cout << "EXITING ORIGINS\n";
						
						break;
					}
					// If the string is empty, it was just a white space. Continue.
					if(currentLine == string()){continue;}
					// Find the position of the equal sign:
					std::size_t posEqual  = currentLine.find("=");
					// The property we want to set:
					std::string propName  = currentLine.substr(0,posEqual);
					// The property name the user gave:
					std::string propGiven = currentLine.substr(posEqual+1,currentLine.length());

					cout << propName + "=" + propGiven << endl;
					cout << "To compare with " + currentLine << endl;

					if(propName == "ORIGIN_ELECTRO_X"){
						this->origin_Electro_grid[0]= std::stod(propGiven);
						
					}else if(propName == "ORIGIN_ELECTRO_Y"){
						this->origin_Electro_grid[1] = std::stod(propGiven);
					
					}else if(propName == "ORIGIN_ELECTRO_Z"){
						this->origin_Electro_grid[2] = std::stod(propGiven);

					}else if(propName == "ORIGIN_THERMAL_X"){
						this->origin_Thermal_grid[0] = std::stod(propGiven);

					}else if(propName == "ORIGIN_THERMAL_Y"){
						this->origin_Thermal_grid[1] = std::stod(propGiven);

					}else if(propName == "ORIGIN_THERMAL_Z"){
						this->origin_Thermal_grid[2] = std::stod(propGiven);

					}else{
						fprintf(stderr,"In %s (%d) :: Cannot associate %s to any property. Aborting.\n",
							__FUNCTION__,__LINE__,propName.c_str());
						fprintf(stderr,"File %s:%d\n",__FILE__,__LINE__);
						abort();
					}
				}
				break;

			default:
				printf("Should not end up here. Complain to Romin. Abort.");
				printf("(in file %s at %d)\n",__FILE__,__LINE__);
				std::cout << "Faulty line is " + currentLine << endl << endl;
				abort();
		}
	}
}

void InputParser::readHeader_RUN_INFOS(ifstream &file){
	std::string currentLine = string();
	while(currentLine != "RUN_INFOS"){
		// Read line:
		getline(file,currentLine);
		// Get rid of comments:
		this->checkLineISNotComment(file,currentLine);
		// Remove any blank space:
		this->RemoveAnyBlankSpaceInStr(currentLine);
		// Remove Dollar sign:
		currentLine = currentLine.substr(currentLine.find("$")+1);
		cout << "BEFORE THE SWITCH : " + currentLine << endl;
		if(currentLine == "RUN_INFOS"){
			cout << "BREAKING FROM RUN_INFOS" << endl;
			break;}
		switch(this->hashit_Header2(currentLine)){
			case STOP_SIMUL_AFTER:
				cout << "Entering case STOP_SIMUL_AFTER\n";
				while(!file.eof()){
					// Note: sections are ended by $the-section-name.
					// Read line:
					getline(file,currentLine);
					// Get rid of comments:
					this->checkLineISNotComment(file,currentLine);
					// Remove any blank in the string:
					this->RemoveAnyBlankSpaceInStr(currentLine);
					cout << currentLine << endl;
					// If the string is "$DELTAS" it means the section ends.
					if(currentLine == "$STOP_SIMUL_AFTER"){
						cout << "EXITING STOP_SIMUL_AFTER\n";
						break;
					}
					// If the string is empty, it was just a white space. Continue.
					if(currentLine == string()){continue;}
					// Find the position of the equal sign:
					std::size_t posEqual  = currentLine.find("=");
					// The property we want to set:
					std::string propName  = currentLine.substr(0,posEqual);
					// The property name the user gave:
					std::string propGiven = currentLine.substr(posEqual+1,currentLine.length());

					cout << propName + "=" + propGiven << endl;
					cout << "To compare with " + currentLine << endl;

					if(propName == "stopTime"){
						this->stopTime = std::stod(propGiven);
						cout << "ADDED stopTime is " << this->stopTime << endl;

					}else if(propName == "maxStepsForOneCycleOfElectro"){
						/**
						 * This property is usefull to impose a maximum number of steps
						 * for the electromagnetic solver before stopping.
						 */
						/// Transform the string in a size_t with std::stold and a cast:
						this->maxStepsForOneCycleOfElectro = (size_t) std::stold(propGiven);;
						printf("Max electro steps is : %zu (from %s)....\n",
							this->maxStepsForOneCycleOfElectro,
							propGiven.c_str());
						if(this->maxStepsForOneCycleOfElectro == 0){
							fprintf(stderr,"In %s ::maxStepsForOneCycleOfElectro has been set to zero. Aborting.\n",
								__FUNCTION__);
							fprintf(stderr,"File %s:%d\n",__FILE__,__LINE__);
							#ifdef MPI_COMM_WORLD
							MPI_Abort(MPI_COMM_WORLD,-1);
							#else
							abort();
							#endif
						}
						
					}else if(propName != "stopTime"){
						printf("InputParser::readHeader_RUN_INFOS:: You didn't provide a ");
						printf("good member for $RUN_INFOS$STOP_SIMUL_AFTER.\nAborting.\n");
						cout << propName << endl;
						printf("(in file %s at %d)\n",__FILE__,__LINE__);
						abort();
					}
				}
				break;

			case TEMP_INIT:
				cout << "Entering case TEMP_INIT\n";
				while(!file.eof()){
					// Note: sections are ended by $the-section-name.
					// Read line:
					getline(file,currentLine);
					// Get rid of comments:
					this->checkLineISNotComment(file,currentLine);
					// Remove any blank in the string:
					this->RemoveAnyBlankSpaceInStr(currentLine);
					cout << currentLine << endl;
					// If the string is "$DELTAS" it means the section ends.
					if(currentLine == "$TEMP_INIT"){
						cout << "EXITING TEMP_INIT\n";
						break;
					}
					// If the string is empty, it was just a white space. Continue.
					if(currentLine == string()){continue;}
					// Find the position of the equal sign:
					std::size_t posEqual  = currentLine.find("=");
					// The property we want to set:
					std::string propName  = currentLine.substr(0,posEqual);
					// The property name the user gave:
					std::string propGiven = currentLine.substr(posEqual+1,currentLine.length());

					cout << propName + "=" + propGiven << endl;
					cout << "To compare with " + currentLine << endl;

					// Create a substring with the material:
					std::string mat = propName.substr(
							propName.find("T_INIT")+sizeof("T_INIT"));
					cout << "SUBSTR IS " + mat << endl;

					if(mat != string()){
						double tempInit = std::stod(propGiven);
						this->GetInitTemp_FromMaterialName.insert(
							std::pair<std::string,double>(mat,tempInit)
						);
						cout << "For material " + mat;
						cout << ", we have initial temperature of ";
						cout << this->GetInitTemp_FromMaterialName[mat] << endl;
					}else{
						printf("InputParser::readHeader_RUN_INFOS:: You didn't provide a ");
						printf("good member for $RUN_INFOS$TEMP_INIT.\nAborting.\n");
						cout << propName << endl;
						printf("(in file %s at %d)\n",__FILE__,__LINE__);
						abort();
					}
				}
				break;

			default:
				printf("Should not end up here. Complain to Romin. Abort.");
				printf("(in file %s at %d)\n",__FILE__,__LINE__);
				std::cout << "Faulty line is " + currentLine << endl << endl;
				abort();
		}
	}
}

std::vector<double> InputParser::determineVectorFromStr(
	std::string str,
	size_t size_to_verify_for /* = 0 */){
	std::stringstream stream(str);
	std::string word;
	std::vector<double> tempVec;
	while( getline(stream, word, ';') ){
		tempVec.push_back(std::stod(word));
	}

	if(tempVec.size() > size_to_verify_for){
		fprintf(stderr,"In %s :: The vector from the string contains more elements than annouced.\n",
			__FUNCTION__);
		fprintf(stderr,"Received %s, annouced %zu elements but has %zu elements. Aborting.\n",
			str.c_str(),size_to_verify_for,tempVec.size());
		abort();
	}
		
	return tempVec;
}