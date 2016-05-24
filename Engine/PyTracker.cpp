/* ***** BEGIN LICENSE BLOCK *****
 * This file is part of Natron <http://www.natron.fr/>,
 * Copyright (C) 2016 INRIA and Alexandre Gauthier-Foichat
 *
 * Natron is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Natron is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Natron.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
 * ***** END LICENSE BLOCK ***** */

// ***** BEGIN PYTHON BLOCK *****
// from <https://docs.python.org/3/c-api/intro.html#include-files>:
// "Since Python may define some pre-processor definitions which affect the standard headers on some systems, you must include Python.h before any standard headers are included."
#include <Python.h>
// ***** END PYTHON BLOCK *****

#include "PyTracker.h"

#include "Engine/PyNode.h"
#include "Engine/TrackMarker.h"
#include "Engine/TrackerContext.h"


NATRON_NAMESPACE_ENTER;
NATRON_PYTHON_NAMESPACE_ENTER;

Track::Track(const boost::shared_ptr<TrackMarker>& marker)
    : _marker(marker)
{
}

Track::~Track()
{
}

void
Track::setScriptName(const QString& scriptName)
{
    _marker->setScriptName( scriptName.toStdString() );
}

QString
Track::getScriptName() const
{
    return QString::fromUtf8( _marker->getScriptName_mt_safe().c_str() );
}

Param*
Track::getParam(const QString& scriptName) const
{
    KnobPtr knob = _marker->getKnobByName( scriptName.toStdString() );

    if (!knob) {
        return 0;
    }
    Param* ret = Effect::createParamWrapperForKnob(knob);

    return ret;
}

void
Track::reset()
{
    _marker->resetTrack();
}

Tracker::Tracker(const boost::shared_ptr<TrackerContext>& ctx)
    : _ctx(ctx)
{
}

Tracker::~Tracker()
{
}

Track*
Tracker::getTrackByName(const QString& name) const
{
    TrackMarkerPtr t = _ctx->getMarkerByName( name.toStdString() );

    if (t) {
        return new Track(t);
    } else {
        return 0;
    }
}

void
Tracker::startTracking(const std::list<Track*>& marks,
                       int start,
                       int end,
                       bool forward)
{
    std::list<TrackMarkerPtr> markers;

    for (std::list<Track*>::const_iterator it = marks.begin(); it != marks.end(); ++it) {
        markers.push_back( (*it)->getInternalMarker() );
    }
    _ctx->trackMarkers(markers, start, end, forward, 0);
}

void
Tracker::stopTracking()
{
    _ctx->abortTracking();
}

void
Tracker::getAllTracks(std::list<Track*>* tracks) const
{
    std::vector<TrackMarkerPtr> markers;

    _ctx->getAllMarkers(&markers);
    for (std::vector<TrackMarkerPtr>::const_iterator it = markers.begin(); it != markers.end(); ++it) {
        tracks->push_back( new Track(*it) );
    }
}

void
Tracker::getSelectedTracks(std::list<Track*>* tracks) const
{
    std::list<TrackMarkerPtr> markers;

    _ctx->getSelectedMarkers(&markers);
    for (std::list<TrackMarkerPtr>::const_iterator it = markers.begin(); it != markers.end(); ++it) {
        tracks->push_back( new Track(*it) );
    }
}

NATRON_PYTHON_NAMESPACE_EXIT;
NATRON_NAMESPACE_EXIT;