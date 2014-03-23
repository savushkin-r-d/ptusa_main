#include "modbus_serv.h"

#include "lua_manager.h"

int ModbusServ::confirmRecUpdateCtr[11] = {0};
char ModbusServ::updateRecFlag[11] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int ModbusServ::confirmPrgUpdateCtr[11] = {0};
char ModbusServ::updatePrgFlag[11] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

extern int isMsa;

unsigned char ModbusServ::UTable[128][2] =
    {
    {0x4,0x2},
    {0x4,0x3},
    {0x20,0x1A},
    {0x4,0x53},
    {0x20,0x1E},
    {0x20,0x26},
    {0x20,0x20},
    {0x20,0x21},
    {0x20,0xAC},
    {0x20,0x30},
    {0x4,0x9},
    {0x20,0x39},
    {0x4,0xA},
    {0x4,0xC},
    {0x4,0xB},
    {0x4,0xF},
    {0x4,0x52},
    {0x20,0x18},
    {0x20,0x19},
    {0x20,0x1C},
    {0x20,0x1D},
    {0x20,0x22},
    {0x20,0x13},
    {0x20,0x14},
    {0x0,0x0},
    {0x21,0x22},
    {0x4,0x59},
    {0x20,0x3A},
    {0x4,0x5A},
    {0x4,0x5C},
    {0x4,0x5B},
    {0x4,0x5F},
    {0x0,0xA0},
    {0x4,0xE},
    {0x4,0x5E},
    {0x4,0x8},
    {0x0,0xA4},
    {0x4,0x90},
    {0x0,0xA6},
    {0x0,0xA7},
    {0x4,0x1},
    {0x0,0xA9},
    {0x4,0x4},
    {0x0,0xAB},
    {0x0,0xAC},
    {0x0,0xAD},
    {0x0,0xAE},
    {0x4,0x7},
    {0x0,0xB0},
    {0x0,0xB1},
    {0x4,0x6},
    {0x4,0x56},
    {0x4,0x91},
    {0x0,0xB5},
    {0x0,0xB6},
    {0x0,0xB7},
    {0x4,0x51},
    {0x21,0x16},
    {0x4,0x54},
    {0x0,0xBB},
    {0x4,0x58},
    {0x4,0x5},
    {0x4,0x55},
    {0x4,0x57},
    {0x4,0x10},
    {0x4,0x11},
    {0x4,0x12},
    {0x4,0x13},
    {0x4,0x14},
    {0x4,0x15},
    {0x4,0x16},
    {0x4,0x17},
    {0x4,0x18},
    {0x4,0x19},
    {0x4,0x1A},
    {0x4,0x1B},
    {0x4,0x1C},
    {0x4,0x1D},
    {0x4,0x1E},
    {0x4,0x1F},
    {0x4,0x20},
    {0x4,0x21},
    {0x4,0x22},
    {0x4,0x23},
    {0x4,0x24},
    {0x4,0x25},
    {0x4,0x26},
    {0x4,0x27},
    {0x4,0x28},
    {0x4,0x29},
    {0x4,0x2A},
    {0x4,0x2B},
    {0x4,0x2C},
    {0x4,0x2D},
    {0x4,0x2E},
    {0x4,0x2F},
    {0x4,0x30},
    {0x4,0x31},
    {0x4,0x32},
    {0x4,0x33},
    {0x4,0x34},
    {0x4,0x35},
    {0x4,0x36},
    {0x4,0x37},
    {0x4,0x38},
    {0x4,0x39},
    {0x4,0x3A},
    {0x4,0x3B},
    {0x4,0x3C},
    {0x4,0x3D},
    {0x4,0x3E},
    {0x4,0x3F},
    {0x4,0x40},
    {0x4,0x41},
    {0x4,0x42},
    {0x4,0x43},
    {0x4,0x44},
    {0x4,0x45},
    {0x4,0x46},
    {0x4,0x47},
    {0x4,0x48},
    {0x4,0x49},
    {0x4,0x4A},
    {0x4,0x4B},
    {0x4,0x4C},
    {0x4,0x4D},
    {0x4,0x4E},
    {0x4,0x4F}
    };

long ModbusServ::ModbusService( long len, unsigned char *data,unsigned char *outdata )
    {
    lua_State* L = lua_manager::get_instance()->get_Lua();
       
    switch ( data[1] ) //Modbus command
        {

        case 0x01: //Read Coils
            {            
            unsigned int startingAddress  = data[2] * 256 + data[3];
            unsigned int numberofElements = data[4] * 256 + data[5];
            unsigned char numberofBytes   = numberofElements / 8;
            if ( numberofElements % 8 ) numberofBytes++;
            outdata[ 2 ] = numberofBytes;
                        
            unsigned int coilgroup = data[0];

			if (isMsa)
				{
				} 
			else
				{
				static char has_Lua_read_coils = 0;
				if ( has_Lua_read_coils == 0 )
					{
					lua_getfield( L, LUA_GLOBALSINDEX, "read_coils" );

					if ( lua_isfunction( L, -1 ) )
						{
						has_Lua_read_coils = 2;
						}
					else
						{
						has_Lua_read_coils = 1;
						}

					lua_remove( L, -1 );  // Stack: remove function "read_coils".
					}

				int i = 0;
				if ( has_Lua_read_coils == 2 )
					{
					lua_getfield( L, LUA_GLOBALSINDEX, "read_coils" );
					lua_pushnumber( L, coilgroup );
					lua_pushnumber( L, startingAddress );
					lua_pushnumber( L, numberofElements );

					int i_line = lua_pcall( L, 3, 1, 0 );

					if ( i_line != 0 )
						{
						Print( "Evaluate Modbus service error: %s\n",
							lua_tostring( L, -1 ) );

						lua_pop( L, 1 );  
						return 0;
						}

					if ( lua_istable( L, -1 ) )
						{
						lua_pushnil( L );
						while( lua_next( L, -2 ) )
							{
							int bit_res = ( int ) lua_tonumber( L, -1 );
							lua_pop( L, 1 );
							ForceBit( i, &outdata[ 3 ], bit_res );
							i++;
							}                                 
						}      
					lua_pop( L, 1 );  
					}
				}

            return 3+numberofBytes;
            }

        case 0x03: //Read Holding Registers
            {            
            unsigned int startingAddress = data[2] * 256 + data[3];
            unsigned int numberofElements = data[4] * 256 + data[5];
            outdata[ 2 ] = (numberofElements * 2) & 0xFF;           
            unsigned int coilgroup = data[0];

			if (isMsa)
				{
				unsigned int i,j,k;
				unsigned int objnumber, line;
				for (i = 0; i < numberofElements; i++)
					{
					objnumber = i + startingAddress;
					switch (coilgroup)
						{
						case C_MSA_RECIPES:

							line = (i + startingAddress) / 3000 + 1;
							if (line > u_int(cipline_tech_object::MdlsCNT))
								{
								continue;
								}
							objnumber = (i+startingAddress) - (line - 1) * 3000;
							switch (objnumber)
								{
								case RC_RECIPE_CONTROL:
									PackInt16(0,&outdata[3+i*2]);
									break;
								case RC_RECIPE_CONTROL_NO:
									PackInt16(int_2(cipline_tech_object::Mdls[line - 1]->rt_par_float[P_CUR_REC]),&outdata[3+i*2]);
									break;
								case RC_SELECTED_REC:
									CP1251toUnicode(cipline_tech_object::Mdls[line - 1]->loadedRecName, &outdata[3+i*2]);
									i+=TRecipeManager::recipeNameLength - 1;
									break;
								case RC_SELECTED_PRG:
									CP1251toUnicode(cipline_tech_object::Mdls[line - 1]->currentProgramName, &outdata[3+i*2]);
									i+= PROGRAM_MAX_LEN - 1;
									break;
								case RC_EDITED_REC:
									CP1251toUnicode(cipline_tech_object::Mdls[line - 1]->lineRecipes->currentRecipeName, &outdata[3+i*2]);
									i+=TRecipeManager::recipeNameLength - 1;
									break;
								case RC_SELECT_REC:
									PackInt16(0,&outdata[3+i*2]);
									break;
								case RC_LIST_UPDATE:
									PackInt16(updateRecFlag[line],&outdata[3+i*2]);
									break;
								case RC_LIST_LENGTH:
									k = 0;
									for (j = 0; j < u_int(cipline_tech_object::Mdls[line - 1]->lineRecipes->recipePerLine); j++)
										{
										if (1 == cipline_tech_object::Mdls[line - 1]->lineRecipes->getRecipeValue(j, TRecipeManager::RV_IS_USED))
											{
											k++;
											}
										}
									PackInt16(k,&outdata[3+i*2]);
									break;
								case RC_PRG_SELECT:
									PackInt16(0,&outdata[3+i*2]);
									break;
								case RC_PRG_UPDATE:
									PackInt16(updatePrgFlag[line],&outdata[3+i*2]);
									break;
								case RC_PRG_LENGTH:
									PackInt16(cipline_tech_object::Mdls[line-1]->prgListLen,&outdata[3+i*2]);
									break;
								default:
									if (objnumber >= RC_LIST_START && objnumber < RC_PRG_SELECT)
										{
										int recipeno = (objnumber - RC_LIST_START) / TRecipeManager::recipeNameLength;
										//								Print("\n\rRecipeNo = %d", recipeno);
										k = -1;
										for (j = 0; j < u_int(cipline_tech_object::Mdls[line - 1]->lineRecipes->recipePerLine); j++)
											{
											if (1 == cipline_tech_object::Mdls[line - 1]->lineRecipes->getRecipeValue(j, TRecipeManager::RV_IS_USED))
												{
												k++;
												}
											if (k == recipeno)
												{
												char myrec[MAX_REC_NAME_LENGTH];
												cipline_tech_object::Mdls[line - 1]->lineRecipes->getRecipeName(j, myrec);
												CP1251toUnicode(myrec, &outdata[3+i*2]);
												break;
												}
											}
										i+= TRecipeManager::recipeNameLength - 1;
										}
									else
										{
										if (objnumber >= RC_PRG_START && objnumber < RC_RECIPE_PAR_START)
											{
											int prgno = (objnumber - RC_PRG_START) / PROGRAM_MAX_LEN;
											CP1251toUnicode(cipline_tech_object::Mdls[line - 1]->prgArray[prgno], &outdata[3+i*2]);
											i+= PROGRAM_MAX_LEN - 1;
											} 
										else
											{
											if (objnumber >= RC_RECIPE_PAR_START)
												{
												k = (objnumber - RC_RECIPE_PAR_START) / 2;
												if (k < u_int(cipline_tech_object::Mdls[line - 1]->lineRecipes->GetParamsCount()))
													{
													PackFloat(cipline_tech_object::Mdls[line - 1]->lineRecipes->getValue(k - 1), &outdata[3+i*2]);
													i++;
													}
												} 
											else
												{
#ifdef DEBUG 
												Print("\n\rRead Unsigned register");
#endif
												}
											}
										}
									break;
								} 
							break;
						default:
							break;
						}
					}
				}
			else
				{
				int idx = 0;
				static char has_Lua_read_holding_registers = 0;
				if ( has_Lua_read_holding_registers == 0 )
					{
					lua_getfield( L, LUA_GLOBALSINDEX, "read_holding_registers" );

					if ( lua_isfunction( L, -1 ) )
						{
						has_Lua_read_holding_registers = 2;
						}
					else
						{
						has_Lua_read_holding_registers = 1;
						}

					lua_remove( L, -1 );  // Stack: remove function "read_holding_registers".
					}

				if ( has_Lua_read_holding_registers == 2 )
					{
					lua_getfield( L, LUA_GLOBALSINDEX, "read_holding_registers" );
					lua_pushnumber( L, coilgroup );
					lua_pushnumber( L, startingAddress );
					lua_pushnumber( L, numberofElements );

					int i_line = lua_pcall( L, 3, 1, 0 );

					if ( i_line != 0 )
						{
						Print( "Evaluate Modbus service error: %s\n",
							lua_tostring( L, -1 ) );

						lua_pop( L, 1 );  
						return 0;
						}

					if ( lua_istable( L, -1 ) )
						{
						lua_pushnil( L );
						while( lua_next( L, -2 ) )
							{
							int t = ( int ) lua_tonumber( L, -1 );
							lua_pop( L, 1 );

							if ( lua_next( L, -2 ) )
								{
								switch ( t )
									{
									case 1: // int16
										{
										int val = ( int ) lua_tonumber( L, -1 );
										PackInt16( val, &outdata[ 3 + idx * 2 ] );
										break;
										}

									case 2: // float                                    
										{
										float val = ( float ) lua_tonumber( L, -1 );
										PackFloat( val, &outdata[ 3 + idx * 2 ] );
										break;
										}

									case 3: // time                                    
										{
										u_long val = ( u_long ) lua_tonumber( L, -1 );
										PackTime( val, &outdata[ 3 + idx * 2 ] );                                    
										break;
										}

									case 4: // String                                    
										{
										const char *val = lua_tostring( L, -1 );
										CP1251toUnicode( val, &outdata[ 3 + idx * 2 ] );                       
										break;
										}
									}   

								lua_pop( L, 1 );
								}
							idx++;
							}     

						lua_pop( L, 1 );
						}  
					else
						{
						Print( "Evaluate Modbus service error: %s\n",
							lua_tostring( L, -1 ) );

						lua_pop( L, 1 );  
						return 0; 
						}                
					}    
				}

            return 3+numberofElements*2;
            }

        case 0x05: //Write Single Coil
            {
            unsigned int startingAddress = data[ 2 ] * 256 + data[ 3 ];            
            unsigned int coilgroup       = data[ 0 ];
            int value                    = data[ 4 ] > 0 ? 1 : 0;

			if (isMsa)
				{
				}
			else
				{
				static char has_Lua_write_coils = 0;
				if ( has_Lua_write_coils == 0 )
					{
					lua_getfield( L, LUA_GLOBALSINDEX, "write_coils" );

					if ( lua_isfunction( L, -1 ) )
						{
						has_Lua_write_coils = 2;
						}
					else
						{
						has_Lua_write_coils = 1;
						}

					lua_remove( L, -1 );  // Stack: remove function "write_coils".
					}

				if ( has_Lua_write_coils == 2 )
					{
					lua_getfield( L, LUA_GLOBALSINDEX, "write_coils" );
					lua_pushnumber( L, coilgroup );
					lua_pushnumber( L, startingAddress );
					lua_pushnumber( L, value );

					int i_line = lua_pcall( L, 3, 0, 0 );

					if ( i_line != 0 )
						{
						Print( "Evaluate Modbus service error: %s\n",
							lua_tostring( L, -1 ) );

						lua_pop( L, 1 );  
						return 0;
						}                
					}
				}

            return 6;
            }

        case 0x10: //Force Multiply Registers
            {
            unsigned int startingAddress  = data[2] * 256 + data[3];
            unsigned int numberofElements = data[4] * 256 + data[5];            
            unsigned int coilgroup        = data[ 0 ];

			if (isMsa)
				{
				}
			else
				{
				static char has_Lua_write_coils = 0;
				if ( has_Lua_write_coils == 0 )
					{
					lua_getfield( L, LUA_GLOBALSINDEX, "write_holding_registers" );

					if ( lua_isfunction( L, -1 ) )
						{
						has_Lua_write_coils = 2;
						}
					else
						{
						has_Lua_write_coils = 1;
						}

					lua_remove( L, -1 );  // Stack: remove function "write_holding_registers".
					}

				if ( has_Lua_write_coils == 2 )
					{
					lua_getfield( L, LUA_GLOBALSINDEX, "write_holding_registers" );
					lua_pushnumber( L, coilgroup );
					lua_pushnumber( L, startingAddress );
					lua_pushnumber( L, numberofElements );
					lua_pushlstring( L, ( const char* ) outdata + 7, 2 * numberofElements );

					int i_line = lua_pcall( L, 4, 0, 0 );

					if ( i_line != 0 )
						{
						Print( "Evaluate Modbus service error: %s\n",
							lua_tostring( L, -1 ) );

						lua_pop( L, 1 );  
						return 0;
						}                
					}
				}

            return 7;
            }
        }

    return 0;
    }

int ModbusServ::ForceBit(int bit, unsigned char* Buf, int toset)
    {
    int bitpos,bytepos;
    bitpos = bit % 8;
    bytepos = bit / 8;
    if (toset)
        {
        Buf[bytepos] = Buf[bytepos] | (1 << bitpos);
        }
    else
        {
        Buf[bytepos] = Buf[bytepos] & (0xFF ^ (1 << bitpos));
        }
    return 0;
    }

int ModbusServ::PackString( char* String, unsigned char* Buf )
    {
    int i = 0;
    while (0 != String[i])
        {
        if (i % 2)
            {
            Buf[i-1] = String[i];
            }
        else
            {
            Buf[i+1] = String[i];
            }
        i++;
        }
    if (i % 2)
        {
        Buf[i-1] = ' ';
        i++;
        }
    return i;
    }

int ModbusServ::PackTime( unsigned long timevar, unsigned char* Buf, int units /*= 0*/ )
    {
    unsigned long temptime;
    unsigned int seconds, minites, hours;
    if (units)
        {
        temptime = timevar;
        }
    else
        {
        temptime = timevar / 1000;
        }
    seconds = temptime % 60;
    minites = (temptime / 60) % 60;
    hours = (temptime / 3600) % 100;
    sprintf((char*)Buf,"%.1hd%.1hd%.1hd::%.1hd%.1hd%.1hd",
        hours % 10, hours / 10, minites / 10, minites % 10, seconds % 10, seconds / 10);
    return 3;
    }

int ModbusServ::PackFloat( float fvalue, unsigned char* Buf )
    {
    Buf[0] = ((char*)&fvalue)[1];
    Buf[1] = ((char*)&fvalue)[0];
    Buf[2] = ((char*)&fvalue)[3];
    Buf[3] = ((char*)&fvalue)[2];
    return 1;
    }

int ModbusServ::PackWord( unsigned int wvalue,char* Buf )
    {
    Buf[0] = ((char*)&wvalue)[1];
    Buf[1] = ((char*)&wvalue)[0];
    return 0;
    }

int ModbusServ::PackLong( unsigned long lvalue,char* Buf )
    {
    Buf[0] = ((char*)&lvalue)[1];
    Buf[1] = ((char*)&lvalue)[0];
    Buf[2] = ((char*)&lvalue)[3];
    Buf[3] = ((char*)&lvalue)[2];
    return 1;
    }

float ModbusServ::UnpackFloat( unsigned char* Buf )
    {
    float result;
    ((char*)&result)[0] = Buf[1];
    ((char*)&result)[1] = Buf[0];
    ((char*)&result)[2] = Buf[3];
    ((char*)&result)[3] = Buf[2];
    return result;
    }

unsigned int ModbusServ::UnpackWord( unsigned char* Buf )
    {
    unsigned int result;
    ((char*)&result)[0] = Buf[1];
    ((char*)&result)[1] = Buf[0];
    return result;
    }

unsigned long ModbusServ::UnpackLong( unsigned char* Buf )
    {
    unsigned long result;
    ((char*)&result)[0] = Buf[1];
    ((char*)&result)[1] = Buf[0];
    ((char*)&result)[2] = Buf[3];
    ((char*)&result)[3] = Buf[2];
    return result;
    }

int_2 ModbusServ::PackInt16( int_2 wvalue,unsigned char* Buf )
    {
    Buf[0] = ((char*)&wvalue)[1];
    Buf[1] = ((char*)&wvalue)[0];
    return 0;
    }

int_2 ModbusServ::UnpackInt16( unsigned char* Buf )
    {
    int_2 result;
    ((char*)&result)[0] = Buf[1];
    ((char*)&result)[1] = Buf[0];
    return result;
    }

int ModbusServ::CP1251toUnicode( const char* Input, unsigned char* Buf)
    {
	int inputlen;
	inputlen = strlen( Input );

    char input_end = 0;
    int i;
    for (i = 0; i <= inputlen; i++)
        {
        unsigned char mychar = (unsigned char)Input[i];
        if (i == inputlen)
            {
            input_end = 1;
            }
        if (input_end)
            {
            Buf[i*2] = 0;
            Buf[i*2 + 1] = 0;
            }
        else
            {
            if (mychar < 128)
                {
                ((unsigned char*)Buf)[i*2] = 0;
                ((unsigned char*)Buf)[i*2 + 1] = mychar;
                }
            else
                {
                ((unsigned char*)Buf)[i*2] = UTable[mychar - 128][0];
                ((unsigned char*)Buf)[i*2 + 1] = UTable[mychar - 128][1];
                }

            }
#ifdef DEBUG
        //Print("\n\r%d. %d %d", i, Buf[i*2], Buf[i*2 + 1]);
#endif //DEBUG
        }
    return 0;
    }

void ModbusServ::UpdateRecipes()
	{
	for (int i = 0; i < 11; i++)
		{
		confirmRecUpdateCtr[i] = 0;
		updateRecFlag[i] = 1;
		}
	}

void ModbusServ::UpdatePrograms()
	{
	for (int i = 0; i < 11; i++)
		{
		confirmPrgUpdateCtr[i] = 0;
		updatePrgFlag[i] = 1;
		}
	}

void ModbusServ::UpdateLineRecipes( int line )
	{
	if (line > 0 && line < 11)
		{
		confirmRecUpdateCtr[line] = 0;
		updateRecFlag[line] = 1;
		}
	}

void ModbusServ::UpdateLinePrograms( int line )
	{
	if (line > 0 && line < 11)
		{
		confirmPrgUpdateCtr[line] = 0;
		updatePrgFlag[line] = 1;
		}
	}


int ModbusServ::UnicodetoCP1251( char* Output, unsigned char* Buf, int inputlen )
    {
    int i;
    unsigned char upperbyte;
    unsigned char lowerbyte;
    for (i = 0; i < inputlen; i++)
        {
        upperbyte = Buf[2 * i];
        lowerbyte = Buf[2 * i + 1];
#ifdef DEBUG
        Print("\n\r%d symbol codes: %d %d", i + 1, upperbyte, lowerbyte);
#endif //DEBUG

        if (i == inputlen - 1 || (0 == upperbyte && 0 == lowerbyte))
            {
            Output[i] = 0;
            break;
            }
        else
            {
            if (0 == upperbyte && lowerbyte < 128)
                {
                Output[i] = lowerbyte;
                }
            else
                {
                for (int j = 0; j<128; j++)
                    {
                    if (UTable[j][0] == upperbyte && UTable[j][1] == lowerbyte)
                        {
                        Output[i] = 128 + j;
                        break;
                        }
                    }
                }

            }
        }
    return i/2 - 1;
    }
