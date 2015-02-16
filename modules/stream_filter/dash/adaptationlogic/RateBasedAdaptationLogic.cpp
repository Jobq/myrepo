/*
 * RateBasedAdaptationLogic.cpp
 *****************************************************************************
 * Copyright (C) 2010 - 2011 Klagenfurt University
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
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "RateBasedAdaptationLogic.h"
#include <iostream>

using namespace std;
using namespace dash::logic;
using namespace dash::xml;
using namespace dash::http;
using namespace dash::mpd;
using namespace dash::exception;

RateBasedAdaptationLogic::RateBasedAdaptationLogic  (IMPDManager *mpdManager) :
    AbstractAdaptationLogic( mpdManager ),
    mpdManager( mpdManager ),
    count( 0 ),
    currentPeriod( mpdManager->getFirstPeriod() )
{
}

Chunk*  RateBasedAdaptationLogic::getNextChunk() throw(EOFException)
{

    cout << "RATE BASED ADAPT LOGIC HERE" << endl;
    if(this->mpdManager == NULL)
        throw EOFException();

    if(this->currentPeriod == NULL)
        throw EOFException();

    long bitrate = this->getBpsAvg();

    Representation *rep = this->mpdManager->getRepresentation(this->currentPeriod, bitrate);

    if ( rep == NULL )
        throw EOFException();

    std::vector<Segment *> segments = this->mpdManager->getSegments(rep);

    if ( this->count == segments.size() )
    {
        this->currentPeriod = this->mpdManager->getNextPeriod(this->currentPeriod);
        this->count = 0;
        return this->getNextChunk();
    }

    if ( segments.size() > this->count )
    {
        Segment *seg = segments.at( this->count );
        Chunk *chunk = new Chunk;
        chunk->setUrl( seg->getSourceUrl() );
        //In case of UrlTemplate, we must stay on the same segment.
        if ( seg->isSingleShot() == true )
            this->count++;
        seg->done();
        return chunk;
    }
    return NULL;
}


Chunk* RateBasedAdaptationLogic::doTheThing() throw(EOFException){

    cout << "rateBasedAdaptationLogic : ___ Doing the thing" << endl;
    if(this->mpdManager == NULL)
        throw EOFException();

    if(this->currentPeriod == NULL)
        throw EOFException();

    long bitrate = this->getBpsAvg();


    /* Find all the representations */
    std::vector<Group *>    groups = this->currentPeriod->getGroups();
    Representation  *best = NULL;
    std::vector<Representation *> reps = groups.at(0)->getRepresentations();
    Representation * rep = reps.at(0);
    Representation * rep2 = reps.at(1);

    /* Find the two next segments to be downloaded */
    for( size_t j = 0; j < reps.size(); j++ )
    {
        ;;
    }

    //Representation *rep = this->mpdManager->getRepresentation(this->currentPeriod, bitrate);

    if ( rep == NULL )
        throw EOFException();

    if ( rep2 == NULL )
        throw EOFException();

    std::vector<Segment *> segments = this->mpdManager->getSegments(rep);
    std::vector<Segment *> segments2 = this->mpdManager->getSegments(rep2);

    if ( this->count == segments.size() )
    {
        this->currentPeriod = this->mpdManager->getNextPeriod(this->currentPeriod);
        this->count = 0;
        return this->doTheThing();
    }

    if ( segments.size() > this->count )
    {
        Segment *seg = segments.at( this->count );
        Segment *seg2 = segments2.at( this->count );

        Chunk * chunkTab[2];

        Chunk *chunk = new Chunk;
        Chunk *chunk2 = new Chunk;

        chunk->setUrl( seg->getSourceUrl() );
        cout << "doTheThing : Source URL of seg1 is : " << seg->getSourceUrl() << endl;
        chunk2->setUrl( seg2->getSourceUrl() );
        cout << "doTheThing : Source URL of seg2 is : " << seg2->getSourceUrl() << endl;

        chunkTab[0] = chunk;
        chunkTab[1] = chunk2;

        //In case of UrlTemplate, we must stay on the same segment.
        if ( seg->isSingleShot() == true )
            this->count++;
        seg->done();
        return (Chunk *)chunkTab;
    }
    return NULL;
}
