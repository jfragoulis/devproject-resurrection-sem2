//*****************************************************************************
//**
//**   File:               SoundBuffer.cpp
//**   Author:             $Author: Joep.Moritz $
//**   Last Change:        $Date: 2008-03-09 14:34:51 +0000 (Sun, 09 Mar 2008) $
//**   Last Revision:      $Revision: 126 $
//**
//*****************************************************************************

#include "Sound.h"

Sound::Sound():
m_buffer(0) 
{}

Sound::~Sound()
{ 
    alDeleteBuffers(1, &m_buffer); 
}

bool Sound::load( const std::string &filename )
{
    // Load wav data into a buffer.
    m_buffer = alutCreateBufferFromFile( filename.c_str() );

    if( m_buffer == AL_NONE ) return AL_FALSE;

    return AL_TRUE;
}