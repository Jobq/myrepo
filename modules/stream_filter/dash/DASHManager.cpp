/*****************************************************************************
 * DASHManager.cpp
 *****************************************************************************
 * Copyright © 2010 - 2011 Klagenfurt University
 *
 * Created on: Aug 10, 2010
 * Authors: Christopher Mueller <christopher.mueller@itec.uni-klu.ac.at>
 *          Christian Timmerer  <christian.timmerer@itec.uni-klu.ac.at>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "DASHManager.h"

using namespace std;
using namespace dash;
using namespace dash::http;
using namespace dash::xml;
using namespace dash::logic;
using namespace dash::mpd;
using namespace dash::exception;

DASHManager::DASHManager    ( HTTPConnectionManager *conManager, MPD *mpd,
                              IAdaptationLogic::LogicType type ) :
    conManager( conManager ),
    currentChunk( NULL ),
    adaptationLogic( NULL ),
    logicType( type ),
    mpdManager( NULL ),
    mpd( mpd )
{
    this->mpdManager        = mpd::MPDManagerFactory::create( mpd );
    if ( this->mpdManager == NULL )
        return ;
    this->adaptationLogic   = AdaptationLogicFactory::create( this->logicType, this->mpdManager );
    if ( this->adaptationLogic == NULL )
        return ;
    this->conManager->attach(this->adaptationLogic);
    this->des = 1;
}
DASHManager::~DASHManager   ()
{
    delete this->adaptationLogic;
    delete this->mpdManager;
}

int     DASHManager::read( void *p_buffer, size_t len )
{

    Chunk * chunk1;
    Chunk * chunk2;
    if ( this->currentChunk == NULL )
    {
        //cout << "entered yes!" << endl;
        try
        {


    /*        Chunk *chunk = new Chunk();
            chunk->setUrl(this->adaptationLogic->schedule.at(0)->getSourceUrl());
            this->currentChunk = chunk;*/
            //this->currentChunk = this->adaptationLogic->getNextChunk();

            /* get the two possible next chunk */
            Chunk ** res;
            res = (Chunk **)this->adaptationLogic->doTheThing();
            
            /* Stre them seperatly */
            chunk1 = res[0];
            chunk2 = res[1];
            this->currentChunk = chunk1;
            this->currentChunk2 = chunk2;
        }
        catch(EOFException &e)
        {
            this->currentChunk = NULL;
            return 0;
        }
    }

    /* Create and run two threads in order to download the content */
    /* get the start time */
    /* wait until the duration of the segments */
    /* perform plaback at palyback time with available chunks, or wait for a thread to be terminated */

    /* Download one out of two segments in each descirption file */
    int ret = 0;
    if (this->des == 0)
    {
        ret = this->conManager->read(this->currentChunk, p_buffer, len);
        cout << "Read 1 : length of the buffer : len = " << len << endl;
        if (ret < 0){
            cout << "read : ret is 0 for des 1!" << endl;
            ret = this->conManager->read(this->currentChunk2, p_buffer, len);
        }

    }
    else if (this->des == 1)
    {
        ret = this->conManager->read(this->currentChunk2, p_buffer, len);
        cout << "Read 2 : length of the buffer : len = " << len << endl;
        if (ret < 0){
            cout << "read : ret is less than 0 for des 2!" << endl;
            ret = this->conManager->read(this->currentChunk, p_buffer, len);
            cout << "Ret is now " << ret << endl;
        }

    }
    else
        cout << "dashManager::read : PBL in the description number while downloading" << endl;


    //char s_buffer[len];
    
   // int ret = this->conManager->read(this->currentChunk, (void *)p_buffer , len);
    //int ret1 = this->conManager->read(this->currentChunk2, (void *)p_buffer , len);
    
    if ( ret == 0 )
    {
        cout << "Read1 : Looking for a new chunk to download" << endl;
        this->currentChunk = NULL;
        this->currentChunk2 = NULL;

        /* Switching chunk to download */
        if (this->des == 1 )
            this->des = 0;
        else if (this->des == 0)
            this->des = 1;
        else
            cout << "dashManager::read : PBL in the description number!!" << endl;

        return this->read(p_buffer, len );
    }
    //cout << "Download yes!" << endl;


    return ret;
}

int     DASHManager::read2( void *p_buffer, size_t len )
{

    //Chunk * chunk1;
    Chunk * chunk2;
    if ( this->currentChunk2 == NULL )
    {
        //cout << "entered yes!" << endl;
        try
        {


    /*        Chunk *chunk = new Chunk();
            chunk->setUrl(this->adaptationLogic->schedule.at(0)->getSourceUrl());
            this->currentChunk = chunk;*/
/*            Chunk *chunk2 = new Chunk();
            chunk2->setUrl(this->adaptationLogic->schedule.at(1)->getSourceUrl());
*/
            //this->currentChunk = this->adaptationLogic->getNextChunk();
            Chunk ** res;
            res = (Chunk **)this->adaptationLogic->doTheThing();
            
            //chunk1 = res[0];
            chunk2 = res[1];
            this->currentChunk2 = chunk2;
            //this->currentChunk = this->adaptationLogic->doTheThing();
  //          cout << "DONE" << endl;
        }
        catch(EOFException &e)
        {
            this->currentChunk2 = NULL;
            return 0;
        }
    }

    /* Create and run two threads in order to download the content */

    /* get the start time */


    /* wait until the duration of the segments */

    /* perform plaback at palyback time with available chunks, or wait for a thread to be terminated */

    //int ret = this->conManager->read(this->currentChunk2, p_buffer, len);
    cout << "Read2 : length of the buffer : len = " << len << endl;
    
    
    int ret2 = this->conManager->read(this->currentChunk2, (void *)p_buffer , len);
    
    if ( ret2 == 0)
    {
        cout << "Read2 : Looking for a new chunk to download" << endl;
        this->currentChunk2 = NULL;
        return this->read(p_buffer, len );
    }
    //cout << "Download yes!" << endl;


    return ret2;
}



int     DASHManager::peek( const uint8_t **pp_peek, size_t i_peek )
{

    Chunk * chunk1;
    Chunk * chunk2;

    if ( this->currentChunk == NULL )
    {
        try
        {

            Chunk ** res;
            res = (Chunk **)this->adaptationLogic->doTheThing();
            
            chunk1 = res[0];
            chunk2 = res[1];
            this->currentChunk = chunk1;
            this->currentChunk2 = chunk2;
 //           this->currentChunk = this->adaptationLogic->getNextChunk();
        }
        catch(EOFException &e)
        {
            return 0;
        }
    }
    //uint8_t ** ss_peek;
    int ret = 0;
    //int ret1 = this->conManager->peek( this->currentChunk2, pp_peek, i_peek );
    /*ret = this->conManager->peek( this->currentChunk2, pp_peek, i_peek );
    cout << "ret is " << ret << endl;
    if (ret <= 0){
        cout << "new try" << endl;
        ret = this->conManager->peek( this->currentChunk, pp_peek, i_peek );
        cout << "ret is now" << ret << endl;
    }*/

    if (this->des == 0)
    {
        ret = this->conManager->peek( this->currentChunk2, pp_peek, i_peek );
        cout << "test Ret is " << ret << endl;
        if (ret <= 0){
            ret = this->conManager->peek( this->currentChunk, pp_peek, i_peek );
            cout << "test2 Ret is " << ret << endl;

        }
    }
    else if (this->des == 1)
    {
        ret = this->conManager->peek( this->currentChunk, pp_peek, i_peek );
        cout << "test Ret is " << ret << endl; 
        if (ret <= 0){
            ret = this->conManager->peek( this->currentChunk2, pp_peek, i_peek );
            cout << "test2 Ret is " << ret << endl;

        }
    }
    else
        cout << "dashManager::read : PBL in the description number while downloading" << endl;

    cout << "Ret is " << ret << endl;


    return ret;
}

const mpd::IMPDManager*         DASHManager::getMpdManager() const
{
    return this->mpdManager;
}

const logic::IAdaptationLogic*  DASHManager::getAdaptionLogic() const
{
    return this->adaptationLogic;
}
