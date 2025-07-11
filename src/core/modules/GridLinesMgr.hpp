/*
 * Stellarium
 * Copyright (C) 2007 Fabien Chereau
 * Copyright (C) 2015 Georg Zotti (more grids to illustrate precession issues)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
 */

#ifndef GRIDLINESMGR_HPP
#define GRIDLINESMGR_HPP

#include <QFont>
#include "VecMath.hpp"
#include "StelModule.hpp"
#include "Planet.hpp"
#include "StelCore.hpp"

class SkyGrid;
class SkyPoint;

//! @class SkyLine
//! Class which manages a line to display around the sky like the ecliptic line.
class SkyLine
{
public:
	enum SKY_LINE_TYPE
	{
		EQUATOR_J2000,
		EQUATOR_OF_DATE,
		FIXED_EQUATOR,
		ECLIPTIC_J2000,
		ECLIPTIC_OF_DATE,
		ECLIPTIC_WITH_DATE,
		PRECESSIONCIRCLE_N,
		PRECESSIONCIRCLE_S,
		MERIDIAN,
		HORIZON,
		GALACTICEQUATOR,
		SUPERGALACTICEQUATOR,
		LONGITUDE,
		QUADRATURE,
		PRIME_VERTICAL,
		CURRENT_VERTICAL,
		COLURE_1,
		COLURE_2,
		CIRCUMPOLARCIRCLE_N,
		CIRCUMPOLARCIRCLE_S,
		INVARIABLEPLANE,
		SOLAR_EQUATOR,
		EARTH_UMBRA,
		EARTH_PENUMBRA,
		ECLIPTIC_CULTURAL,
		EQUATORIAL_CULTURAL
	};
	// Create and precompute positions of a SkyGrid
	SkyLine(SKY_LINE_TYPE _line_type = EQUATOR_J2000);
	virtual ~SkyLine();
	static void init(); //! call once before creating the first line.
	static void deinit(); //! call once after deleting all lines.
	void draw(StelCore* core) const;       // set up a painter and draw
	void draw(StelPainter &painter, const float oldLineWidth) const; // draw with given painter
	void setColor(const Vec3f& c) {color = c;}
	void setPartitions(bool visible) {showPartitions = visible;}
	bool showsPartitions() const {return showPartitions;}
	const Vec3f& getColor() const {return color;}
	void update(double deltaTime) {fader.update(static_cast<int>(deltaTime*1000));}
	void setFadeDuration(float duration) {fader.setDuration(static_cast<int>(duration*1000.f));}
	void setDisplayed(const bool displayed){fader = displayed;}
	bool isDisplayed() const {return fader;}
	void setLabeled(const bool displayed){showLabel = displayed;}
	bool isLabeled() const {return showLabel;}
	void setFontSize(int newSize);
	void setLineThickness(const float thickness) {lineThickness = thickness;}
	float getLineThickness() const {return lineThickness;}
	void setPartThickness(const float thickness) {partThickness = thickness;}
	float getPartThickness() const {return partThickness;}
	//! Re-translates the label and sets the frameType. Must be called in the constructor!
	void updateLabel();
	//! setup the small partitions in a ECLIPTIC_CULTURAL or EQUATORIAL_CULTURAL line, in degrees.
	//! Element nr.0 is a vector of the major divisions. It is not used directly, just here for completion.
	//! Element nr.1 is a vector of all main divisions of all major divisions. This may be 10-degrees in a 12x30 degree zodiac, or the quarter-lines in the Indian 27-part nakshatras.
	//! Element nr.2 is a vector of all minor divisions, like the 5-degrees in a 12x30 degrees zodiac defined as [12 3 2 5].
	//! Element nr.3 is a vector of all sub-minor divisions, like the 1-degrees in a 12x30 degrees zodiac  defined as [12 3 2 5].
	//! Up to these 4 lists is supported with drawing ever-smaller sub-ticks
	//! @todo: Currently the partitions are falsely plotted extending the wrong axis.
	void setCulturalPartitions(QList<QList<double>>cParts){culturalPartitions=cParts;}
	void setCulturalOffset(const double offset){culturalOffset=offset;}
	static void setSolarSystem(SolarSystem* ss);
	//! Compute eclipticOnDatePartitions for @param year. Trigger a call to this from a signal StelCore::dateChangedByYear()
	static void computeEclipticDatePartitions(int year = std::numeric_limits<int>::min());
private:
	static QSharedPointer<Planet> earth, sun, moon;
	SKY_LINE_TYPE line_type;
	Vec3f color;
	StelCore::FrameType frameType;
	LinearFader fader;
	QFont font;
	QString label;
	float lineThickness;
	float partThickness;
	bool showPartitions;
	bool showLabel;
	QList<QList<double>>culturalPartitions; //!< only in ECLIPTIC_CULTURAL and EQUATORIAL_CULTURAL lines.
	double culturalOffset;  //!< the origin of cultural partitions can be rotated from the first point of Aries.
	static QMap<int, double> precessionPartitions;
	static std::vector<QPair<Vec3d, QString>> eclipticOnDatePartitions; //!< Collection of up to 366 entries Vec3d={eclLongitude, aberration, nutation}, QString label
};


//! @class GridLinesMgr
//! The GridLinesMgr controls the drawing of the Azimuthal, Equatorial, Ecliptical and Galactic Grids,
//! as well as the great circles: Meridian Line, Ecliptic Lines of J2000.0 and date, Equator Line (of J2000.0 and date),
//! Solar Equator and Invariable Plane of the Solar System,
//! Precession Circles, and a special line marking conjunction or opposition in ecliptical longitude (of date).
class GridLinesMgr : public StelModule
{
	Q_OBJECT
	Q_PROPERTY(bool gridlinesDisplayed 		READ getFlagGridlines		WRITE setFlagGridlines			NOTIFY gridlinesDisplayedChanged)

	Q_PROPERTY(bool azimuthalGridDisplayed		READ getFlagAzimuthalGrid	WRITE setFlagAzimuthalGrid		NOTIFY azimuthalGridDisplayedChanged)
	Q_PROPERTY(Vec3f azimuthalGridColor		READ getColorAzimuthalGrid	WRITE setColorAzimuthalGrid		NOTIFY azimuthalGridColorChanged)

	Q_PROPERTY(bool equatorGridDisplayed		READ getFlagEquatorGrid		WRITE setFlagEquatorGrid		NOTIFY equatorGridDisplayedChanged)
	Q_PROPERTY(Vec3f equatorGridColor		READ getColorEquatorGrid	WRITE setColorEquatorGrid		NOTIFY equatorGridColorChanged)

	Q_PROPERTY(bool fixedEquatorGridDisplayed	READ getFlagFixedEquatorGrid	WRITE setFlagFixedEquatorGrid		NOTIFY fixedEquatorGridDisplayedChanged)
	Q_PROPERTY(Vec3f fixedEquatorGridColor		READ getColorFixedEquatorGrid	WRITE setColorFixedEquatorGrid		NOTIFY fixedEquatorGridColorChanged)

	Q_PROPERTY(bool equatorJ2000GridDisplayed	READ getFlagEquatorJ2000Grid	WRITE setFlagEquatorJ2000Grid		NOTIFY equatorJ2000GridDisplayedChanged)
	Q_PROPERTY(Vec3f equatorJ2000GridColor		READ getColorEquatorJ2000Grid	WRITE setColorEquatorJ2000Grid		NOTIFY equatorJ2000GridColorChanged)

	Q_PROPERTY(bool eclipticJ2000GridDisplayed	READ getFlagEclipticJ2000Grid	WRITE setFlagEclipticJ2000Grid		NOTIFY eclipticJ2000GridDisplayedChanged)
	Q_PROPERTY(Vec3f eclipticJ2000GridColor		READ getColorEclipticJ2000Grid	WRITE setColorEclipticJ2000Grid		NOTIFY eclipticJ2000GridColorChanged)

	Q_PROPERTY(bool eclipticGridDisplayed		READ getFlagEclipticGrid	WRITE setFlagEclipticGrid		NOTIFY eclipticGridDisplayedChanged)
	Q_PROPERTY(Vec3f eclipticGridColor		READ getColorEclipticGrid	WRITE setColorEclipticGrid		NOTIFY eclipticGridColorChanged)

	Q_PROPERTY(bool galacticGridDisplayed		READ getFlagGalacticGrid	WRITE setFlagGalacticGrid		NOTIFY galacticGridDisplayedChanged)
	Q_PROPERTY(Vec3f galacticGridColor		READ getColorGalacticGrid	WRITE setColorGalacticGrid		NOTIFY galacticGridColorChanged)

	Q_PROPERTY(bool supergalacticGridDisplayed	READ getFlagSupergalacticGrid	WRITE setFlagSupergalacticGrid		NOTIFY supergalacticGridDisplayedChanged)
	Q_PROPERTY(Vec3f supergalacticGridColor		READ getColorSupergalacticGrid	WRITE setColorSupergalacticGrid		NOTIFY supergalacticGridColorChanged)

	Q_PROPERTY(bool equatorLineDisplayed		READ getFlagEquatorLine		WRITE setFlagEquatorLine		NOTIFY equatorLineDisplayedChanged)
	Q_PROPERTY(bool equatorPartsDisplayed		READ getFlagEquatorParts	WRITE setFlagEquatorParts		NOTIFY equatorPartsDisplayedChanged)
	Q_PROPERTY(bool equatorPartsLabeled		READ getFlagEquatorLabeled	WRITE setFlagEquatorLabeled		NOTIFY equatorPartsLabeledChanged)
	Q_PROPERTY(Vec3f equatorLineColor		READ getColorEquatorLine	WRITE setColorEquatorLine		NOTIFY equatorLineColorChanged)

	Q_PROPERTY(bool equatorJ2000LineDisplayed	READ getFlagEquatorJ2000Line	WRITE setFlagEquatorJ2000Line		NOTIFY equatorJ2000LineDisplayedChanged)
	Q_PROPERTY(bool equatorJ2000PartsDisplayed	READ getFlagEquatorJ2000Parts	WRITE setFlagEquatorJ2000Parts		NOTIFY equatorJ2000PartsDisplayedChanged)
	Q_PROPERTY(bool equatorJ2000PartsLabeled	READ getFlagEquatorJ2000Labeled	WRITE setFlagEquatorJ2000Labeled	NOTIFY equatorJ2000PartsLabeledChanged)
	Q_PROPERTY(Vec3f equatorJ2000LineColor		READ getColorEquatorJ2000Line	WRITE setColorEquatorJ2000Line		NOTIFY equatorJ2000LineColorChanged)

	Q_PROPERTY(bool fixedEquatorLineDisplayed	READ getFlagFixedEquatorLine	WRITE setFlagFixedEquatorLine		NOTIFY fixedEquatorLineDisplayedChanged)
	Q_PROPERTY(bool fixedEquatorPartsDisplayed	READ getFlagFixedEquatorParts	WRITE setFlagFixedEquatorParts		NOTIFY fixedEquatorPartsDisplayedChanged)
	Q_PROPERTY(bool fixedEquatorPartsLabeled	READ getFlagFixedEquatorLabeled	WRITE setFlagFixedEquatorLabeled	NOTIFY fixedEquatorPartsLabeledChanged)
	Q_PROPERTY(Vec3f fixedEquatorLineColor		READ getColorFixedEquatorLine	WRITE setColorFixedEquatorLine		NOTIFY fixedEquatorLineColorChanged)

	Q_PROPERTY(bool eclipticLineDisplayed		READ getFlagEclipticLine	WRITE setFlagEclipticLine		NOTIFY eclipticLineDisplayedChanged)
	Q_PROPERTY(bool eclipticPartsDisplayed		READ getFlagEclipticParts	WRITE setFlagEclipticParts		NOTIFY eclipticPartsDisplayedChanged)
	Q_PROPERTY(bool eclipticPartsLabeled		READ getFlagEclipticLabeled	WRITE setFlagEclipticLabeled		NOTIFY eclipticPartsLabeledChanged)
	Q_PROPERTY(bool eclipticDatesLabeled           READ getFlagEclipticDatesLabeled WRITE setFlagEclipticDatesLabeled	NOTIFY eclipticDatesLabeledChanged)
	Q_PROPERTY(Vec3f eclipticLineColor		READ getColorEclipticLine	WRITE setColorEclipticLine		NOTIFY eclipticLineColorChanged)

	Q_PROPERTY(bool eclipticJ2000LineDisplayed	READ getFlagEclipticJ2000Line	 WRITE setFlagEclipticJ2000Line		NOTIFY eclipticJ2000LineDisplayedChanged)
	Q_PROPERTY(bool eclipticJ2000PartsDisplayed	READ getFlagEclipticJ2000Parts	 WRITE setFlagEclipticJ2000Parts	NOTIFY eclipticJ2000PartsDisplayedChanged)
	Q_PROPERTY(bool eclipticJ2000PartsLabeled	READ getFlagEclipticJ2000Labeled WRITE setFlagEclipticJ2000Labeled	NOTIFY eclipticJ2000PartsLabeledChanged)
	Q_PROPERTY(Vec3f eclipticJ2000LineColor		READ getColorEclipticJ2000Line	 WRITE setColorEclipticJ2000Line	NOTIFY eclipticJ2000LineColorChanged)

	Q_PROPERTY(bool invariablePlaneLineDisplayed	READ getFlagInvariablePlaneLine	 WRITE setFlagInvariablePlaneLine	NOTIFY invariablePlaneLineDisplayedChanged)
	Q_PROPERTY(Vec3f invariablePlaneLineColor	READ getColorInvariablePlaneLine WRITE setColorInvariablePlaneLine	NOTIFY invariablePlaneLineColorChanged)

	Q_PROPERTY(bool solarEquatorLineDisplayed	READ getFlagSolarEquatorLine	 WRITE setFlagSolarEquatorLine		NOTIFY solarEquatorLineDisplayedChanged)
	Q_PROPERTY(bool solarEquatorPartsDisplayed	READ getFlagSolarEquatorParts	 WRITE setFlagSolarEquatorParts		NOTIFY solarEquatorPartsDisplayedChanged)
	Q_PROPERTY(bool solarEquatorPartsLabeled	READ getFlagSolarEquatorLabeled  WRITE setFlagSolarEquatorLabeled	NOTIFY solarEquatorPartsLabeledChanged)
	Q_PROPERTY(Vec3f solarEquatorLineColor		READ getColorSolarEquatorLine	 WRITE setColorSolarEquatorLine		NOTIFY solarEquatorLineColorChanged)

	Q_PROPERTY(bool precessionCirclesDisplayed	READ getFlagPrecessionCircles	WRITE setFlagPrecessionCircles		NOTIFY precessionCirclesDisplayedChanged)
	Q_PROPERTY(bool precessionPartsDisplayed	READ getFlagPrecessionParts	WRITE setFlagPrecessionParts		NOTIFY precessionPartsDisplayedChanged)
	Q_PROPERTY(bool precessionPartsLabeled		READ getFlagPrecessionLabeled	WRITE setFlagPrecessionLabeled		NOTIFY precessionPartsLabeledChanged)
	Q_PROPERTY(Vec3f precessionCirclesColor		READ getColorPrecessionCircles	WRITE setColorPrecessionCircles		NOTIFY precessionCirclesColorChanged)

	Q_PROPERTY(bool meridianLineDisplayed		READ getFlagMeridianLine	WRITE setFlagMeridianLine		NOTIFY meridianLineDisplayedChanged)
	Q_PROPERTY(bool meridianPartsDisplayed		READ getFlagMeridianParts	WRITE setFlagMeridianParts		NOTIFY meridianPartsDisplayedChanged)
	Q_PROPERTY(bool meridianPartsLabeled		READ getFlagMeridianLabeled	WRITE setFlagMeridianLabeled		NOTIFY meridianPartsLabeledChanged)
	Q_PROPERTY(Vec3f meridianLineColor		READ getColorMeridianLine	WRITE setColorMeridianLine		NOTIFY meridianLineColorChanged)

	Q_PROPERTY(bool longitudeLineDisplayed	READ getFlagLongitudeLine	WRITE setFlagLongitudeLine		NOTIFY longitudeLineDisplayedChanged)
	Q_PROPERTY(bool longitudePartsDisplayed	READ getFlagLongitudeParts	WRITE setFlagLongitudeParts		NOTIFY longitudePartsDisplayedChanged)
	Q_PROPERTY(bool longitudePartsLabeled	READ getFlagLongitudeLabeled	WRITE setFlagLongitudeLabeled		NOTIFY longitudePartsLabeledChanged)
	Q_PROPERTY(Vec3f longitudeLineColor		READ getColorLongitudeLine	WRITE setColorLongitudeLine		NOTIFY longitudeLineColorChanged)

	Q_PROPERTY(bool quadratureLineDisplayed		READ getFlagQuadratureLine	WRITE setFlagQuadratureLine		NOTIFY quadratureLineDisplayedChanged)
	Q_PROPERTY(Vec3f quadratureLineColor		READ getColorQuadratureLine	WRITE setColorQuadratureLine		NOTIFY quadratureLineColorChanged)

	Q_PROPERTY(bool horizonLineDisplayed		READ getFlagHorizonLine		WRITE setFlagHorizonLine		NOTIFY horizonLineDisplayedChanged)
	Q_PROPERTY(bool horizonPartsDisplayed	READ getFlagHorizonParts	WRITE setFlagHorizonParts		NOTIFY horizonPartsDisplayedChanged)
	Q_PROPERTY(bool horizonPartsLabeled		READ getFlagHorizonLabeled	WRITE setFlagHorizonLabeled		NOTIFY horizonPartsLabeledChanged)
	Q_PROPERTY(Vec3f horizonLineColor		READ getColorHorizonLine	WRITE setColorHorizonLine		NOTIFY horizonLineColorChanged)

	Q_PROPERTY(bool galacticEquatorLineDisplayed	READ getFlagGalacticEquatorLine		WRITE setFlagGalacticEquatorLine	NOTIFY galacticEquatorLineDisplayedChanged)
	Q_PROPERTY(bool galacticEquatorPartsDisplayed	READ getFlagGalacticEquatorParts	WRITE setFlagGalacticEquatorParts	NOTIFY galacticEquatorPartsDisplayedChanged)
	Q_PROPERTY(bool galacticEquatorPartsLabeled	READ getFlagGalacticEquatorLabeled	WRITE setFlagGalacticEquatorLabeled	NOTIFY galacticEquatorPartsLabeledChanged)
	Q_PROPERTY(Vec3f galacticEquatorLineColor	READ getColorGalacticEquatorLine	WRITE setColorGalacticEquatorLine	NOTIFY galacticEquatorLineColorChanged)

	Q_PROPERTY(bool supergalacticEquatorLineDisplayed	READ getFlagSupergalacticEquatorLine	WRITE setFlagSupergalacticEquatorLine	 NOTIFY supergalacticEquatorLineDisplayedChanged)
	Q_PROPERTY(bool supergalacticEquatorPartsDisplayed	READ getFlagSupergalacticEquatorParts	WRITE setFlagSupergalacticEquatorParts	 NOTIFY supergalacticEquatorPartsDisplayedChanged)
	Q_PROPERTY(bool supergalacticEquatorPartsLabeled	READ getFlagSupergalacticEquatorLabeled	WRITE setFlagSupergalacticEquatorLabeled NOTIFY supergalacticEquatorPartsLabeledChanged)
	Q_PROPERTY(Vec3f supergalacticEquatorLineColor		READ getColorSupergalacticEquatorLine	WRITE setColorSupergalacticEquatorLine	 NOTIFY supergalacticEquatorLineColorChanged)

	Q_PROPERTY(bool primeVerticalLineDisplayed	READ getFlagPrimeVerticalLine	 WRITE setFlagPrimeVerticalLine		NOTIFY primeVerticalLineDisplayedChanged)
	Q_PROPERTY(bool primeVerticalPartsDisplayed	READ getFlagPrimeVerticalParts	 WRITE setFlagPrimeVerticalParts	NOTIFY primeVerticalPartsDisplayedChanged)
	Q_PROPERTY(bool primeVerticalPartsLabeled	READ getFlagPrimeVerticalLabeled WRITE setFlagPrimeVerticalLabeled	NOTIFY primeVerticalPartsLabeledChanged)
	Q_PROPERTY(Vec3f primeVerticalLineColor		READ getColorPrimeVerticalLine	 WRITE setColorPrimeVerticalLine	NOTIFY primeVerticalLineColorChanged)

	Q_PROPERTY(bool currentVerticalLineDisplayed	READ getFlagCurrentVerticalLine	   WRITE setFlagCurrentVerticalLine	NOTIFY currentVerticalLineDisplayedChanged)
	Q_PROPERTY(bool currentVerticalPartsDisplayed	READ getFlagCurrentVerticalParts   WRITE setFlagCurrentVerticalParts	NOTIFY currentVerticalPartsDisplayedChanged)
	Q_PROPERTY(bool currentVerticalPartsLabeled	READ getFlagCurrentVerticalLabeled WRITE setFlagCurrentVerticalLabeled	NOTIFY currentVerticalPartsLabeledChanged)
	Q_PROPERTY(Vec3f currentVerticalLineColor	READ getColorCurrentVerticalLine   WRITE setColorCurrentVerticalLine	NOTIFY currentVerticalLineColorChanged)

	Q_PROPERTY(bool colureLinesDisplayed		READ getFlagColureLines		WRITE setFlagColureLines		NOTIFY colureLinesDisplayedChanged)
	Q_PROPERTY(bool colurePartsDisplayed		READ getFlagColureParts		WRITE setFlagColureParts		NOTIFY colurePartsDisplayedChanged)
	Q_PROPERTY(bool colurePartsLabeled		READ getFlagColureLabeled	WRITE setFlagColureLabeled		NOTIFY colurePartsLabeledChanged)
	Q_PROPERTY(Vec3f colureLinesColor		READ getColorColureLines	WRITE setColorColureLines		NOTIFY colureLinesColorChanged)

	Q_PROPERTY(bool circumpolarCirclesDisplayed	READ getFlagCircumpolarCircles	WRITE setFlagCircumpolarCircles		NOTIFY circumpolarCirclesDisplayedChanged)
	Q_PROPERTY(Vec3f circumpolarCirclesColor	READ getColorCircumpolarCircles	WRITE setColorCircumpolarCircles	NOTIFY circumpolarCirclesColorChanged)

	Q_PROPERTY(bool umbraCircleDisplayed		READ getFlagUmbraCircle		WRITE setFlagUmbraCircle		NOTIFY umbraCircleDisplayedChanged)
	Q_PROPERTY(Vec3f umbraCircleColor		READ getColorUmbraCircle	WRITE setColorUmbraCircle		NOTIFY umbraCircleColorChanged)
	Q_PROPERTY(bool penumbraCircleDisplayed		READ getFlagPenumbraCircle	WRITE setFlagPenumbraCircle		NOTIFY penumbraCircleDisplayedChanged)
	Q_PROPERTY(Vec3f penumbraCircleColor		READ getColorPenumbraCircle	WRITE setColorPenumbraCircle		NOTIFY penumbraCircleColorChanged)

	Q_PROPERTY(bool celestialJ2000PolesDisplayed	READ getFlagCelestialJ2000Poles		WRITE setFlagCelestialJ2000Poles	NOTIFY celestialJ2000PolesDisplayedChanged)
	Q_PROPERTY(Vec3f celestialJ2000PolesColor	READ getColorCelestialJ2000Poles	WRITE setColorCelestialJ2000Poles	NOTIFY celestialJ2000PolesColorChanged)

	Q_PROPERTY(bool celestialPolesDisplayed		READ getFlagCelestialPoles	WRITE setFlagCelestialPoles		NOTIFY celestialPolesDisplayedChanged)
	Q_PROPERTY(Vec3f celestialPolesColor		READ getColorCelestialPoles	WRITE setColorCelestialPoles		NOTIFY celestialPolesColorChanged)

	Q_PROPERTY(bool zenithNadirDisplayed		READ getFlagZenithNadir		WRITE setFlagZenithNadir		NOTIFY zenithNadirDisplayedChanged)
	Q_PROPERTY(Vec3f zenithNadirColor		READ getColorZenithNadir	WRITE setColorZenithNadir		NOTIFY zenithNadirColorChanged)

	Q_PROPERTY(bool eclipticJ2000PolesDisplayed	READ getFlagEclipticJ2000Poles	WRITE setFlagEclipticJ2000Poles		NOTIFY eclipticJ2000PolesDisplayedChanged)
	Q_PROPERTY(Vec3f eclipticJ2000PolesColor	READ getColorEclipticJ2000Poles	WRITE setColorEclipticJ2000Poles	NOTIFY eclipticJ2000PolesColorChanged)

	Q_PROPERTY(bool eclipticPolesDisplayed		READ getFlagEclipticPoles		WRITE setFlagEclipticPoles		NOTIFY eclipticPolesDisplayedChanged)
	Q_PROPERTY(Vec3f eclipticPolesColor		READ getColorEclipticPoles	WRITE setColorEclipticPoles		NOTIFY eclipticPolesColorChanged)

	Q_PROPERTY(bool galacticPolesDisplayed		READ getFlagGalacticPoles	WRITE setFlagGalacticPoles		NOTIFY galacticPolesDisplayedChanged)
	Q_PROPERTY(Vec3f galacticPolesColor		READ getColorGalacticPoles	WRITE setColorGalacticPoles		NOTIFY galacticPolesColorChanged)

	Q_PROPERTY(bool galacticCenterDisplayed		READ getFlagGalacticCenter	WRITE setFlagGalacticCenter		NOTIFY galacticCenterDisplayedChanged)
	Q_PROPERTY(Vec3f galacticCenterColor		READ getColorGalacticCenter	WRITE setColorGalacticCenter		NOTIFY galacticCenterColorChanged)

	Q_PROPERTY(bool supergalacticPolesDisplayed	READ getFlagSupergalacticPoles	WRITE setFlagSupergalacticPoles		NOTIFY supergalacticPolesDisplayedChanged)
	Q_PROPERTY(Vec3f supergalacticPolesColor	READ getColorSupergalacticPoles	WRITE setColorSupergalacticPoles	NOTIFY supergalacticPolesColorChanged)

	Q_PROPERTY(bool equinoxJ2000PointsDisplayed	READ getFlagEquinoxJ2000Points	WRITE setFlagEquinoxJ2000Points		NOTIFY equinoxJ2000PointsDisplayedChanged)
	Q_PROPERTY(Vec3f equinoxJ2000PointsColor	READ getColorEquinoxJ2000Points	WRITE setColorEquinoxJ2000Points	NOTIFY equinoxJ2000PointsColorChanged)

	Q_PROPERTY(bool equinoxPointsDisplayed		READ getFlagEquinoxPoints	WRITE setFlagEquinoxPoints		NOTIFY equinoxPointsDisplayedChanged)
	Q_PROPERTY(Vec3f equinoxPointsColor		READ getColorEquinoxPoints	WRITE setColorEquinoxPoints		NOTIFY equinoxPointsColorChanged)

	Q_PROPERTY(bool solsticeJ2000PointsDisplayed	READ getFlagSolsticeJ2000Points	WRITE setFlagSolsticeJ2000Points	NOTIFY solsticeJ2000PointsDisplayedChanged)
	Q_PROPERTY(Vec3f solsticeJ2000PointsColor	READ getColorSolsticeJ2000Points	WRITE setColorSolsticeJ2000Points	NOTIFY solsticeJ2000PointsColorChanged)

	Q_PROPERTY(bool solsticePointsDisplayed		READ getFlagSolsticePoints	WRITE setFlagSolsticePoints		NOTIFY solsticePointsDisplayedChanged)
	Q_PROPERTY(Vec3f solsticePointsColor		READ getColorSolsticePoints	WRITE setColorSolsticePoints		NOTIFY solsticePointsColorChanged)

	Q_PROPERTY(bool antisolarPointDisplayed		READ getFlagAntisolarPoint	WRITE setFlagAntisolarPoint		NOTIFY antisolarPointDisplayedChanged)
	Q_PROPERTY(Vec3f antisolarPointColor		READ getColorAntisolarPoint	WRITE setColorAntisolarPoint		NOTIFY antisolarPointColorChanged)

	Q_PROPERTY(bool umbraCenterPointDisplayed	READ getFlagUmbraCenterPoint	WRITE setFlagUmbraCenterPoint		NOTIFY umbraCenterPointDisplayedChanged)

	Q_PROPERTY(bool apexPointsDisplayed		READ getFlagApexPoints		WRITE setFlagApexPoints			NOTIFY apexPointsDisplayedChanged)
	Q_PROPERTY(Vec3f apexPointsColor		READ getColorApexPoints		WRITE setColorApexPoints		NOTIFY apexPointsColorChanged)

	Q_PROPERTY(float lineThickness			READ getLineThickness		WRITE setLineThickness			NOTIFY lineThicknessChanged)
	Q_PROPERTY(float partThickness			READ getPartThickness		WRITE setPartThickness			NOTIFY partThicknessChanged)
public:
	GridLinesMgr();
	~GridLinesMgr() override;

	///////////////////////////////////////////////////////////////////////////
	// Methods defined in the StelModule class
	//! Initialize the GridLinesMgr. This process checks the values in the
	//! application settings, and according to the settings there
	//! sets the visibility of the Equatorial Grids, Ecliptical Grids, Azimuthal Grid, Meridian Line,
	//! Equator Line and Ecliptic Lines.
	void init() override;

	//! Get the module ID, returns "GridLinesMgr".
	virtual QString getModuleID() const {return "GridLinesMgr";}

	//! Draw the grids and great circle lines.
	//! Draws the Equatorial Grids, Ecliptical Grids, Azimuthal Grid, Meridian Line, Equator Line,
	//! Ecliptic Lines, Precession Circles, Conjunction-Opposition Line, east-west vertical and colures according to the
	//! various flags which control their visibility.
	void draw(StelCore* core) override;

	//! Update time-dependent features.
	//! Used to control fading when turning on and off the grid lines and great circles.
	void update(double deltaTime) override;

	//! Used to determine the order in which the various modules are drawn.
	double getCallOrder(StelModuleActionName actionName) const override;

	///////////////////////////////////////////////////////////////////////////////////////
	// Setter and getters
public slots:
	//! Setter ("master switch") for displaying any grid/line.
	void setFlagGridlines(const bool displayed);
	//! Accessor ("master switch") for displaying any grid/line.
	bool getFlagGridlines() const;

	//! Setter ("master switch by type") for displaying all grids.
	void setFlagAllGrids(const bool displayed);
	//! Setter ("master switch by type") for displaying all lines.
	void setFlagAllLines(const bool displayed);
	//! Setter ("master switch by type") for displaying all points.
	void setFlagAllPoints(const bool displayed);

	//! Setter for displaying Azimuthal Grid.
	void setFlagAzimuthalGrid(const bool displayed);
	//! Accessor for displaying Azimuthal Grid.
	bool getFlagAzimuthalGrid() const;
	//! Get the current color of the Azimuthal Grid.
	Vec3f getColorAzimuthalGrid() const;
	//! Set the color of the Azimuthal Grid.
	//! @param newColor The color of azimuthal grid
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorAzimuthalGrid(c.toVec3f());
	//! @endcode
	void setColorAzimuthalGrid(const Vec3f& newColor);

	//! Setter for displaying Equatorial Grid.
	void setFlagEquatorGrid(const bool displayed);
	//! Accessor for displaying Equatorial Grid.
	bool getFlagEquatorGrid() const;
	//! Get the current color of the Equatorial Grid.
	Vec3f getColorEquatorGrid() const;
	//! Set the color of the Equatorial Grid.
	//! @param newColor The color of equatorial grid
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorEquatorGrid(c.toVec3f());
	//! @endcode
	void setColorEquatorGrid(const Vec3f& newColor);

	//! Setter for displaying the Fixed Equatorial Grid (Hour angle/declination).
	void setFlagFixedEquatorGrid(const bool displayed);
	//! Accessor for displaying Fixed Equatorial Grid.
	bool getFlagFixedEquatorGrid() const;
	//! Get the current color of the Fixed Equatorial Grid.
	Vec3f getColorFixedEquatorGrid() const;
	//! Set the color of the FixedEquatorial Grid.
	//! @param newColor The color of fixed equatorial grid
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorFixedEquatorGrid(c.toVec3f());
	//! @endcode
	void setColorFixedEquatorGrid(const Vec3f& newColor);

	//! Setter for displaying Equatorial J2000 Grid.
	void setFlagEquatorJ2000Grid(const bool displayed);
	//! Accessor for displaying Equatorial J2000 Grid.
	bool getFlagEquatorJ2000Grid() const;
	//! Get the current color of the Equatorial J2000 Grid.
	Vec3f getColorEquatorJ2000Grid() const;
	//! Set the color of the Equatorial J2000 Grid.
	//! @param newColor The color of equatorial J2000 grid
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorEquatorJ2000Grid(c.toVec3f());
	//! @endcode
	void setColorEquatorJ2000Grid(const Vec3f& newColor);

	//! Setter for displaying Ecliptic Grid of J2000.0.
	void setFlagEclipticJ2000Grid(const bool displayed);
	//! Accessor for displaying Ecliptic Grid.
	bool getFlagEclipticJ2000Grid() const;
	//! Get the current color of the Ecliptic J2000 Grid.
	Vec3f getColorEclipticJ2000Grid() const;
	//! Set the color of the Ecliptic J2000 Grid.
	//! @param newColor The color of ecliptic J2000 grid
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorEclipticJ2000Grid(c.toVec3f());
	//! @endcode
	void setColorEclipticJ2000Grid(const Vec3f& newColor);

	//! Setter for displaying Ecliptic Grid of Date.
	void setFlagEclipticGrid(const bool displayed);
	//! Accessor for displaying Ecliptic Grid.
	bool getFlagEclipticGrid() const;
	//! Get the current color of the Ecliptic of Date Grid.
	Vec3f getColorEclipticGrid() const;
	//! Set the color of the Ecliptic Grid.
	//! @param newColor The color of Ecliptic of Date grid
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorEclipticGrid(c.toVec3f());
	//! @endcode
	void setColorEclipticGrid(const Vec3f& newColor);

	//! Setter for displaying Galactic Grid.
	void setFlagGalacticGrid(const bool displayed);
	//! Accessor for displaying Galactic Grid.
	bool getFlagGalacticGrid() const;
	//! Get the current color of the Galactic Grid.
	Vec3f getColorGalacticGrid() const;
	//! Set the color of the Galactic Grid.
	//! @param newColor The color of galactic grid
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorGalacticGrid(c.toVec3f());
	//! @endcode
	void setColorGalacticGrid(const Vec3f& newColor);

	//! Setter for displaying Supergalactic Grid.
	void setFlagSupergalacticGrid(const bool displayed);
	//! Accessor for displaying Supergalactic Grid.
	bool getFlagSupergalacticGrid() const;
	//! Get the current color of the Supergalactic Grid.
	Vec3f getColorSupergalacticGrid() const;
	//! Set the color of the Supergalactic Grid.
	//! @param newColor The color of supergalactic grid
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorSupergalacticGrid(c.toVec3f());
	//! @endcode
	void setColorSupergalacticGrid(const Vec3f& newColor);

	//! Setter for displaying Equatorial Line.
	void setFlagEquatorLine(const bool displayed);
	//! Accessor for displaying Equatorial Line.
	bool getFlagEquatorLine() const;
	//! Setter for displaying Equatorial line partitions.
	void setFlagEquatorParts(const bool displayed);
	//! Accessor for displaying Equatorial line partitions.
	bool getFlagEquatorParts() const;
	//! Setter for displaying Equatorial line partition labels.
	void setFlagEquatorLabeled(const bool displayed);
	//! Accessor for displaying Equatorial line partition labels.
	bool getFlagEquatorLabeled() const;
	//! Get the current color of the Equatorial Line.
	Vec3f getColorEquatorLine() const;
	//! Set the color of the Equator Line.
	//! @param newColor The color of equator line
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorEquatorLine(c.toVec3f());
	//! @endcode
	void setColorEquatorLine(const Vec3f& newColor);

	//! Setter for displaying J2000 Equatorial Line.
	void setFlagEquatorJ2000Line(const bool displayed);
	//! Accessor for displaying J2000 Equatorial Line.
	bool getFlagEquatorJ2000Line() const;
	//! Setter for displaying J2000 Equatorial line partitions.
	void setFlagEquatorJ2000Parts(const bool displayed);
	//! Accessor for displaying J2000 Equatorial line partitions.
	bool getFlagEquatorJ2000Parts() const;
	//! Setter for displaying J2000 Equatorial line partition labels.
	void setFlagEquatorJ2000Labeled(const bool displayed);
	//! Accessor for displaying J2000 Equatorial line partition labels.
	bool getFlagEquatorJ2000Labeled() const;
	//! Get the current color of the J2000 Equatorial Line.
	Vec3f getColorEquatorJ2000Line() const;
	//! Set the color of the J2000 Equator Line.
	//! @param newColor The color of J2000 equator line
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorEquatorJ2000Line(c.toVec3f());
	//! @endcode
	void setColorEquatorJ2000Line(const Vec3f& newColor);

	//! Setter for displaying Fixed Equator Line.
	void setFlagFixedEquatorLine(const bool displayed);
	//! Accessor for displaying Fixed Equator Line.
	bool getFlagFixedEquatorLine() const;
	//! Setter for displaying Fixed Equator line partitions (hour angles).
	void setFlagFixedEquatorParts(const bool displayed);
	//! Accessor for displaying Fixed Equator (hour angles) line partitions.
	bool getFlagFixedEquatorParts() const;
	//! Setter for displaying Fixed Equator line partition labels.
	void setFlagFixedEquatorLabeled(const bool displayed);
	//! Accessor for displaying Fixed Equator line partition labels.
	bool getFlagFixedEquatorLabeled() const;
	//! Get the current color of the Fixed Equator Line.
	Vec3f getColorFixedEquatorLine() const;
	//! Set the color of the Fixed Equator Line (hour angles).
	//! @param newColor The color of fixed equator line
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorFixedEquatorLine(c.toVec3f());
	//! @endcode
	void setColorFixedEquatorLine(const Vec3f& newColor);

	//! Setter for displaying Ecliptic of J2000 Line.
	void setFlagEclipticJ2000Line(const bool displayed);
	//! Accessor for displaying Ecliptic of J2000 Line.
	bool getFlagEclipticJ2000Line() const;
	//! Setter for displaying Ecliptic of J2000 line partitions.
	void setFlagEclipticJ2000Parts(const bool displayed);
	//! Accessor for displaying Ecliptic of J2000 line partitions.
	bool getFlagEclipticJ2000Parts() const;
	//! Setter for displaying Ecliptic of J2000 line partition labels.
	void setFlagEclipticJ2000Labeled(const bool displayed);
	//! Accessor for displaying Ecliptic of J2000 line partition labels.
	bool getFlagEclipticJ2000Labeled() const;
	//! Get the current color of the Ecliptic of J2000 Line.
	Vec3f getColorEclipticJ2000Line() const;
	//! Set the color of the Ecliptic of J2000 Line.
	//! @param newColor The color of ecliptic 2000 line
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorEcliptic2000Line(c.toVec3f());
	//! @endcode
	void setColorEclipticJ2000Line(const Vec3f& newColor);

	//! Setter for displaying Ecliptic Line.
	void setFlagEclipticLine(const bool displayed);
	//! Accessor for displaying Ecliptic Line.
	bool getFlagEclipticLine() const;
	//! Setter for displaying Ecliptic line partitions.
	void setFlagEclipticParts(const bool displayed);
	//! Accessor for displaying Ecliptic line partitions.
	bool getFlagEclipticParts() const;
	//! Setter for displaying Ecliptic line partition labels.
	void setFlagEclipticLabeled(const bool displayed);
	//! Accessor for displaying Ecliptic line partition labels.
	bool getFlagEclipticLabeled() const;
	//! Setter for displaying Ecliptic line partition labels of dates for Solar position in the current year.
	void setFlagEclipticDatesLabeled(const bool displayed);
	//! Accessor for displaying Ecliptic line partition labels of dates for Solar position in the current year.
	bool getFlagEclipticDatesLabeled() const;

	//! Get the current color of the Ecliptic Line.
	Vec3f getColorEclipticLine() const;
	//! Set the color of the Ecliptic Line.
	//! @param newColor The color of ecliptic line
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorEclipticLine(c.toVec3f());
	//! @endcode
	void setColorEclipticLine(const Vec3f& newColor);

	//! Setter for displaying a line for the Invariable Plane of the Solar System.
	void setFlagInvariablePlaneLine(const bool displayed);
	//! Accessor for displaying Invariable Plane Line.
	bool getFlagInvariablePlaneLine() const;
	//! Get the current color of the Line for the Invariable Plane.
	Vec3f getColorInvariablePlaneLine() const;
	//! Set the color of the Line for the Invariable Plane.
	//! @param newColor The color of the Invariable Plane line
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorInvariablePlaneLine(c.toVec3f());
	//! @endcode
	void setColorInvariablePlaneLine(const Vec3f& newColor);

	//! Setter for displaying Solar Equator Line.
	void setFlagSolarEquatorLine(const bool displayed);
	//! Accessor for displaying Solar Equator Line.
	bool getFlagSolarEquatorLine() const;
	//! Setter for displaying Solar Equator line partitions.
	void setFlagSolarEquatorParts(const bool displayed);
	//! Accessor for displaying Solar Equator line partitions.
	bool getFlagSolarEquatorParts() const;
	//! Setter for displaying Solar Equator line partition labels.
	void setFlagSolarEquatorLabeled(const bool displayed);
	//! Accessor for displaying Solar Equator line partition labels.
	bool getFlagSolarEquatorLabeled() const;
	//! Get the current color of the Solar Equator Line.
	Vec3f getColorSolarEquatorLine() const;
	//! Set the color of the Solar Equator Line.
	//! @param newColor The color of Solar Equator line
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorSolarEquatorLine(c.toVec3f());
	//! @endcode
	void setColorSolarEquatorLine(const Vec3f& newColor);

	//! Setter for displaying precession circles.
	void setFlagPrecessionCircles(const bool displayed);
	//! Accessor for displaying precession circles.
	bool getFlagPrecessionCircles() const;
	//! Setter for displaying precession circle partitions.
	void setFlagPrecessionParts(const bool displayed);
	//! Accessor for displaying precession circle partitions.
	bool getFlagPrecessionParts() const;
	//! Setter for displaying precession circle partition labels.
	void setFlagPrecessionLabeled(const bool displayed);
	//! Accessor for displaying precession circle partition labels.
	bool getFlagPrecessionLabeled() const;
	//! Get the current color of the precession circles.
	Vec3f getColorPrecessionCircles() const;
	//! Set the color of the precession circles.
	//! @param newColor The color of precession circles
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorPrecessionCircles(c.toVec3f());
	//! @endcode
	void setColorPrecessionCircles(const Vec3f& newColor);

	//! Setter for displaying Meridian Line.
	void setFlagMeridianLine(const bool displayed);
	//! Accessor for displaying Meridian Line.
	bool getFlagMeridianLine() const;
	//! Setter for displaying Meridian line partitions.
	void setFlagMeridianParts(const bool displayed);
	//! Accessor for displaying Meridian line partitions.
	bool getFlagMeridianParts() const;
	//! Setter for displaying Meridian line partition labels.
	void setFlagMeridianLabeled(const bool displayed);
	//! Accessor for displaying Meridian line partition labels.
	bool getFlagMeridianLabeled() const;
	//! Get the current color of the Meridian Line.
	Vec3f getColorMeridianLine() const;
	//! Set the color of the Meridian Line.
	//! @param newColor The color of meridian line
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorMeridianLine(c.toVec3f());
	//! @endcode
	void setColorMeridianLine(const Vec3f& newColor);

	//! Setter for displaying opposition/conjunction longitude line.
	void setFlagLongitudeLine(const bool displayed);
	//! Accessor for displaying opposition/conjunction longitude line.
	bool getFlagLongitudeLine() const;
	//! Setter for displaying opposition/conjunction longitude line partitions.
	void setFlagLongitudeParts(const bool displayed);
	//! Accessor for displaying opposition/conjunction longitude line partitions.
	bool getFlagLongitudeParts() const;
	//! Setter for displaying opposition/conjunction longitude line partition labels.
	void setFlagLongitudeLabeled(const bool displayed);
	//! Accessor for displaying opposition/conjunction longitude line partition labels.
	bool getFlagLongitudeLabeled() const;
	//! Get the current color of the opposition/conjunction longitude line.
	Vec3f getColorLongitudeLine() const;
	//! Set the color of the opposition/conjunction longitude line.
	//! @param newColor The color of opposition/conjunction longitude line
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorLongitudeLine(c.toVec3f());
	//! @endcode
	void setColorLongitudeLine(const Vec3f& newColor);

	//! Setter for displaying quadrature line.
	void setFlagQuadratureLine(const bool displayed);
	//! Accessor for displaying quadrature line.
	bool getFlagQuadratureLine() const;
	//! Get the current color of the quadrature line.
	Vec3f getColorQuadratureLine() const;
	//! Set the color of the quadrature line.
	//! @param newColor The color of quadrature line
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorQuadratureLine(c.toVec3f());
	//! @endcode
	void setColorQuadratureLine(const Vec3f& newColor);

	//! Setter for displaying Horizon Line.
	void setFlagHorizonLine(const bool displayed);
	//! Accessor for displaying Horizon Line.
	bool getFlagHorizonLine() const;
	//! Setter for displaying Horizon Line partitions.
	void setFlagHorizonParts(const bool displayed);
	//! Accessor for displaying Horizon Line partitions.
	bool getFlagHorizonParts() const;
	//! Setter for displaying Horizon Line partition labels.
	void setFlagHorizonLabeled(const bool displayed);
	//! Accessor for displaying Horizon Line partition labels.
	bool getFlagHorizonLabeled() const;
	//! Get the current color of the Horizon Line.
	Vec3f getColorHorizonLine() const;
	//! Set the color of the Horizon Line.
	//! @param newColor The color of horizon line
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorHorizonLine(c.toVec3f());
	//! @endcode
	void setColorHorizonLine(const Vec3f& newColor);

	//! Setter for displaying Galactic Equator Line.
	void setFlagGalacticEquatorLine(const bool displayed);
	//! Accessor for displaying Galactic Equator Line.
	bool getFlagGalacticEquatorLine() const;
	//! Setter for displaying Galactic Equator Line partitions.
	void setFlagGalacticEquatorParts(const bool displayed);
	//! Accessor for displaying Galactic Equator Line partitions.
	bool getFlagGalacticEquatorParts() const;
	//! Setter for displaying Galactic Equator Line partition labels.
	void setFlagGalacticEquatorLabeled(const bool displayed);
	//! Accessor for displaying Galactic Equator Line partition labels.
	bool getFlagGalacticEquatorLabeled() const;
	//! Get the current color of the Galactic Equator Line.
	Vec3f getColorGalacticEquatorLine() const;
	//! Set the color of the Galactic Equator Line.
	//! @param newColor The color of galactic equator line
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorGalacticEquatorLine(c.toVec3f());
	//! @endcode
	void setColorGalacticEquatorLine(const Vec3f& newColor);

	//! Setter for displaying Supergalactic Equator Line.
	void setFlagSupergalacticEquatorLine(const bool displayed);
	//! Accessor for displaying Supergalactic Equator Line.
	bool getFlagSupergalacticEquatorLine() const;
	//! Setter for displaying Supergalactic Equator Line partitions.
	void setFlagSupergalacticEquatorParts(const bool displayed);
	//! Accessor for displaying Supergalactic Equator Line partitions.
	bool getFlagSupergalacticEquatorParts() const;
	//! Setter for displaying Supergalactic Equator Line partition labels.
	void setFlagSupergalacticEquatorLabeled(const bool displayed);
	//! Accessor for displaying Supergalactic Equator Line partition labels.
	bool getFlagSupergalacticEquatorLabeled() const;
	//! Get the current color of the Supergalactic Equator Line.
	Vec3f getColorSupergalacticEquatorLine() const;
	//! Set the color of the Supergalactic Equator Line.
	//! @param newColor The color of supergalactic equator line
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorSupergalacticEquatorLine(c.toVec3f());
	//! @endcode
	void setColorSupergalacticEquatorLine(const Vec3f& newColor);

	//! Setter for displaying the Prime Vertical Line.
	void setFlagPrimeVerticalLine(const bool displayed);
	//! Accessor for displaying Prime Vertical Line.
	bool getFlagPrimeVerticalLine() const;
	//! Setter for displaying the Prime Vertical Line partitions.
	void setFlagPrimeVerticalParts(const bool displayed);
	//! Accessor for displaying Prime Vertical Line partitions.
	bool getFlagPrimeVerticalParts() const;
	//! Setter for displaying the Prime Vertical Line partition labels.
	void setFlagPrimeVerticalLabeled(const bool displayed);
	//! Accessor for displaying Prime Vertical Line partition labels.
	bool getFlagPrimeVerticalLabeled() const;
	//! Get the current color of the Prime Vertical Line.
	Vec3f getColorPrimeVerticalLine() const;
	//! Set the color of the Prime Vertical Line.
	//! @param newColor The color of the Prime Vertical line
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorPrimeVerticalLine(c.toVec3f());
	//! @endcode
	void setColorPrimeVerticalLine(const Vec3f& newColor);

	//! Setter for displaying the Current Vertical Line.
	void setFlagCurrentVerticalLine(const bool displayed);
	//! Accessor for displaying Current Vertical Line.
	bool getFlagCurrentVerticalLine() const;
	//! Setter for displaying the Current Vertical Line partitions.
	void setFlagCurrentVerticalParts(const bool displayed);
	//! Accessor for displaying Current Vertical Line partitions.
	bool getFlagCurrentVerticalParts() const;
	//! Setter for displaying the Current Vertical Line partition labels.
	void setFlagCurrentVerticalLabeled(const bool displayed);
	//! Accessor for displaying Current Vertical Line partition labels.
	bool getFlagCurrentVerticalLabeled() const;
	//! Get the current color of the Current Vertical Line.
	Vec3f getColorCurrentVerticalLine() const;
	//! Set the color of the Current Vertical Line.
	//! @param newColor The color of the Current Vertical line
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorCurrentVerticalLine(c.toVec3f());
	//! @endcode
	void setColorCurrentVerticalLine(const Vec3f& newColor);

	//! Setter for displaying the Colure Lines.
	void setFlagColureLines(const bool displayed);
	//! Accessor for displaying the Colure Lines.
	bool getFlagColureLines() const;
	//! Setter for displaying the Colure Line partitions.
	void setFlagColureParts(const bool displayed);
	//! Accessor for displaying the Colure Line partitions.
	bool getFlagColureParts() const;
	//! Setter for displaying the Colure Line partition labels.
	void setFlagColureLabeled(const bool displayed);
	//! Accessor for displaying the Colure Line partitions.
	bool getFlagColureLabeled() const;
	//! Get the current color of the Colure Lines.
	Vec3f getColorColureLines() const;
	//! Set the color of the Colure Lines.
	//! @param newColor The color of the Colure lines
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorColureLines(c.toVec3f());
	//! @endcode
	void setColorColureLines(const Vec3f& newColor);

	//! Setter for displaying circumpolar circles.
	void setFlagCircumpolarCircles(const bool displayed);
	//! Accessor for displaying circumpolar circles.
	bool getFlagCircumpolarCircles() const;
	//! Get the current color of the circumpolar circles.
	Vec3f getColorCircumpolarCircles() const;
	//! Set the color of the circumpolar circles.
	//! @param newColor The color of circumpolar circles
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorCircumpolarCircles(c.toVec3f());
	//! @endcode
	void setColorCircumpolarCircles(const Vec3f& newColor);

	//! Setter for displaying umbra circle.
	void setFlagUmbraCircle(const bool displayed);
	//! Accessor for displaying umbra circle.
	bool getFlagUmbraCircle() const;
	//! Get the current color of the umbra circle.
	Vec3f getColorUmbraCircle() const;
	//! Set the color of the umbra circle.
	//! @param newColor The color of umbra circle
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorUmbraCircle(c.toVec3f());
	//! @endcode
	void setColorUmbraCircle(const Vec3f& newColor);

	//! Setter for displaying penumbra circle.
	void setFlagPenumbraCircle(const bool displayed);
	//! Accessor for displaying penumbra circle.
	bool getFlagPenumbraCircle() const;
	//! Get the current color of the penumbra circle.
	Vec3f getColorPenumbraCircle() const;
	//! Set the color of the penumbra circle.
	//! @param newColor The color of penumbra circle
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorPenumbraCircle(c.toVec3f());
	//! @endcode
	void setColorPenumbraCircle(const Vec3f& newColor);

	//! Setter for displaying celestial poles of J2000.
	void setFlagCelestialJ2000Poles(const bool displayed);
	//! Accessor for displaying celestial poles of J2000.
	bool getFlagCelestialJ2000Poles() const;
	//! Get the current color of the celestial poles of J2000.
	Vec3f getColorCelestialJ2000Poles() const;
	//! Set the color of the celestial poles of J2000.
	//! @param newColor The color of celestial poles of J2000
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorCelestialJ2000Poles(c.toVec3f());
	//! @endcode
	void setColorCelestialJ2000Poles(const Vec3f& newColor);

	//! Setter for displaying celestial poles.
	void setFlagCelestialPoles(const bool displayed);
	//! Accessor for displaying celestial poles.
	bool getFlagCelestialPoles() const;
	//! Get the current color of the celestial poles.
	Vec3f getColorCelestialPoles() const;
	//! Set the color of the celestial poles.
	//! @param newColor The color of celestial poles
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorCelestialPoles(c.toVec3f());
	//! @endcode
	void setColorCelestialPoles(const Vec3f& newColor);

	//! Setter for displaying zenith and nadir.
	void setFlagZenithNadir(const bool displayed);
	//! Accessor for displaying zenith and nadir.
	bool getFlagZenithNadir() const;
	//! Get the current color of the zenith and nadir.
	Vec3f getColorZenithNadir() const;
	//! Set the color of the zenith and nadir.
	//! @param newColor The color of zenith and nadir
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorZenithNadir(c.toVec3f());
	//! @endcode
	void setColorZenithNadir(const Vec3f& newColor);

	//! Setter for displaying ecliptic poles of J2000.
	void setFlagEclipticJ2000Poles(const bool displayed);
	//! Accessor for displaying ecliptic poles of J2000.
	bool getFlagEclipticJ2000Poles() const;
	//! Get the current color of the ecliptic poles of J2000.
	Vec3f getColorEclipticJ2000Poles() const;
	//! Set the color of the ecliptic poles of J2000.
	//! @param newColor The color of ecliptic poles of J2000
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorEclipticJ2000Poles(c.toVec3f());
	//! @endcode
	void setColorEclipticJ2000Poles(const Vec3f& newColor);

	//! Setter for displaying ecliptic poles.
	void setFlagEclipticPoles(const bool displayed);
	//! Accessor for displaying ecliptic poles.
	bool getFlagEclipticPoles() const;
	//! Get the current color of the ecliptic poles.
	Vec3f getColorEclipticPoles() const;
	//! Set the color of the ecliptic poles.
	//! @param newColor The color of ecliptic poles
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorEclipticPoles(c.toVec3f());
	//! @endcode
	void setColorEclipticPoles(const Vec3f& newColor);

	//! Setter for displaying galactic poles.
	void setFlagGalacticPoles(const bool displayed);
	//! Accessor for displaying galactic poles.
	bool getFlagGalacticPoles() const;
	//! Get the current color of the galactic poles.
	Vec3f getColorGalacticPoles() const;
	//! Set the color of the galactic poles.
	//! @param newColor The color of galactic poles
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorGalacticPoles(c.toVec3f());
	//! @endcode
	void setColorGalacticPoles(const Vec3f& newColor);

	//! Setter for displaying galactic center and anticenter markers.
	void setFlagGalacticCenter(const bool displayed);
	//! Accessor for displaying galactic center and anticenter markers.
	bool getFlagGalacticCenter() const;
	//! Get the current color of the galactic center and anticenter markers.
	Vec3f getColorGalacticCenter() const;
	//! Set the color of the galactic center and anticenter markers.
	//! @param newColor The color of galactic center and anticenter markers
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorGalacticCenter(c.toVec3f());
	//! @endcode
	void setColorGalacticCenter(const Vec3f& newColor);

	//! Setter for displaying supergalactic poles.
	void setFlagSupergalacticPoles(const bool displayed);
	//! Accessor for displaying supergalactic poles.
	bool getFlagSupergalacticPoles() const;
	//! Get the current color of the supergalactic poles.
	Vec3f getColorSupergalacticPoles() const;
	//! Set the color of the supergalactic poles.
	//! @param newColor The color of supergalactic poles
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorSupergalacticPoles(c.toVec3f());
	//! @endcode
	void setColorSupergalacticPoles(const Vec3f& newColor);

	//! Setter for displaying equinox points of J2000.
	void setFlagEquinoxJ2000Points(const bool displayed);
	//! Accessor for displaying equinox points of J2000.
	bool getFlagEquinoxJ2000Points() const;
	//! Get the current color of the equinox points of J2000.
	Vec3f getColorEquinoxJ2000Points() const;
	//! Set the color of the equinox points of J2000.
	//! @param newColor The color of equinox points
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorEquinoxJ2000Points(c.toVec3f());
	//! @endcode
	void setColorEquinoxJ2000Points(const Vec3f& newColor);

	//! Setter for displaying equinox points.
	void setFlagEquinoxPoints(const bool displayed);
	//! Accessor for displaying equinox points.
	bool getFlagEquinoxPoints() const;
	//! Get the current color of the equinox points.
	Vec3f getColorEquinoxPoints() const;
	//! Set the color of the equinox points.
	//! @param newColor The color of equinox points
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorEquinoxPoints(c.toVec3f());
	//! @endcode
	void setColorEquinoxPoints(const Vec3f& newColor);

	//! Setter for displaying solstice points of J2000.
	void setFlagSolsticeJ2000Points(const bool displayed);
	//! Accessor for displaying solstice points of J2000.
	bool getFlagSolsticeJ2000Points() const;
	//! Get the current color of the solstice points of J2000.
	Vec3f getColorSolsticeJ2000Points() const;
	//! Set the color of the solstice points of J2000.
	//! @param newColor The color of solstice points
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorSolsticeJ2000Points(c.toVec3f());
	//! @endcode
	void setColorSolsticeJ2000Points(const Vec3f& newColor);

	//! Setter for displaying solstice points.
	void setFlagSolsticePoints(const bool displayed);
	//! Accessor for displaying solstice points.
	bool getFlagSolsticePoints() const;
	//! Get the current color of the solstice points.
	Vec3f getColorSolsticePoints() const;
	//! Set the color of the solstice points.
	//! @param newColor The color of solstice points
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorSolsticePoints(c.toVec3f());
	//! @endcode
	void setColorSolsticePoints(const Vec3f& newColor);

	//! Setter for displaying antisolar point.
	void setFlagAntisolarPoint(const bool displayed);
	//! Accessor for displaying antisolar point.
	bool getFlagAntisolarPoint() const;
	//! Get the current color of the antisolar point.
	Vec3f getColorAntisolarPoint() const;
	//! Set the color of the antisolar point.
	//! @param newColor The color of antisolar point
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorAntisolarPoint(c.toVec3f());
	//! @endcode
	void setColorAntisolarPoint(const Vec3f& newColor);

	//! Setter for displaying the point of the center of umbra.
	void setFlagUmbraCenterPoint(const bool displayed);
	//! Accessor for displaying the point of the center of umbra.
	bool getFlagUmbraCenterPoint() const;

	//! Setter for displaying the Apex and Antapex points, i.e. where the observer planet is heading to or coming from, respectively
	void setFlagApexPoints(const bool displayed);
	//! Accessor for displaying Apex/Antapex points.
	bool getFlagApexPoints() const;
	//! Get the current color of the Apex/Antapex points.
	Vec3f getColorApexPoints() const;
	//! Set the color of the Apex/Antapex points.
	//! @param newColor The color of Apex/Antapex points.
	//! @code
	//! // example of usage in scripts (Qt6-based Stellarium)
	//! var c = new Color(1.0, 0.0, 0.0);
	//! GridLinesMgr.setColorApexPoints(c.toVec3f());
	//! @endcode
	void setColorApexPoints(const Vec3f& newColor);

	//! Set the thickness of lines
	//! @param thickness of line in pixels
	void setLineThickness(const float thickness);
	//! Get the thickness of lines
	float getLineThickness() const;

	//! Set the thickness of partition lines
	//! @param thickness of line in pixels
	void setPartThickness(const float thickness);
	//! Get the thickness of lines
	float getPartThickness() const;

signals:
	void gridlinesDisplayedChanged(const bool);
	void lineThicknessChanged(const float);
	void partThicknessChanged(const float);
	void azimuthalGridDisplayedChanged(const bool);
	void azimuthalGridColorChanged(const Vec3f & newColor);
	void equatorGridDisplayedChanged(const bool displayed);
	void equatorGridColorChanged(const Vec3f & newColor);
	void fixedEquatorGridDisplayedChanged(const bool displayed);
	void fixedEquatorGridColorChanged(const Vec3f & newColor);
	void equatorJ2000GridDisplayedChanged(const bool displayed);
	void equatorJ2000GridColorChanged(const Vec3f & newColor);
	void eclipticGridDisplayedChanged(const bool displayed);
	void eclipticGridColorChanged(const Vec3f & newColor);
	void eclipticJ2000GridDisplayedChanged(const bool displayed);
	void eclipticJ2000GridColorChanged(const Vec3f & newColor);
	void galacticGridDisplayedChanged(const bool displayed);
	void galacticGridColorChanged(const Vec3f & newColor);
	void supergalacticGridDisplayedChanged(const bool displayed);
	void supergalacticGridColorChanged(const Vec3f & newColor);
	void equatorLineDisplayedChanged(const bool displayed);
	void equatorPartsDisplayedChanged(const bool displayed);
	void equatorPartsLabeledChanged(const bool displayed);
	void equatorLineColorChanged(const Vec3f & newColor);
	void equatorJ2000LineDisplayedChanged(const bool displayed);
	void equatorJ2000PartsDisplayedChanged(const bool displayed);
	void equatorJ2000PartsLabeledChanged(const bool displayed);
	void equatorJ2000LineColorChanged(const Vec3f & newColor);
	void fixedEquatorLineDisplayedChanged(const bool displayed);
	void fixedEquatorPartsDisplayedChanged(const bool displayed);
	void fixedEquatorPartsLabeledChanged(const bool displayed);
	void fixedEquatorLineColorChanged(const Vec3f & newColor);
	void eclipticLineDisplayedChanged(const bool displayed);
	void eclipticPartsDisplayedChanged(const bool displayed);
	void eclipticPartsLabeledChanged(const bool displayed);
	void eclipticDatesLabeledChanged(const bool displayed);
	void eclipticLineColorChanged(const Vec3f & newColor);
	void invariablePlaneLineDisplayedChanged(const bool displayed);
	//void invariablePlanePartsDisplayedChanged(const bool displayed);
	//void invariablePlanePartsLabeledChanged(const bool displayed);
	void invariablePlaneLineColorChanged(const Vec3f & newColor);
	void solarEquatorLineDisplayedChanged(const bool displayed);
	void solarEquatorPartsDisplayedChanged(const bool displayed);
	void solarEquatorPartsLabeledChanged(const bool displayed);
	void solarEquatorLineColorChanged(const Vec3f & newColor);
	void eclipticJ2000LineDisplayedChanged(const bool displayed);
	void eclipticJ2000PartsDisplayedChanged(const bool displayed);
	void eclipticJ2000PartsLabeledChanged(const bool displayed);
	void eclipticJ2000LineColorChanged(const Vec3f & newColor);
	void precessionCirclesDisplayedChanged(const bool displayed);
	void precessionPartsDisplayedChanged(const bool displayed);
	void precessionPartsLabeledChanged(const bool displayed);
	void precessionCirclesColorChanged(const Vec3f & newColor);
	void meridianLineDisplayedChanged(const bool displayed);
	void meridianPartsDisplayedChanged(const bool displayed);
	void meridianPartsLabeledChanged(const bool displayed);
	void meridianLineColorChanged(const Vec3f & newColor);
	void longitudeLineDisplayedChanged(const bool displayed);
	void longitudePartsDisplayedChanged(const bool displayed);
	void longitudePartsLabeledChanged(const bool displayed);
	void longitudeLineColorChanged(const Vec3f & newColor);
	void quadratureLineDisplayedChanged(const bool displayed);
	void quadratureLineColorChanged(const Vec3f & newColor);
	void horizonLineDisplayedChanged(const bool displayed);
	void horizonPartsDisplayedChanged(const bool displayed);
	void horizonPartsLabeledChanged(const bool displayed);
	void horizonLineColorChanged(const Vec3f & newColor);
	void galacticEquatorLineDisplayedChanged(const bool displayed);
	void galacticEquatorPartsDisplayedChanged(const bool displayed);
	void galacticEquatorPartsLabeledChanged(const bool displayed);
	void galacticEquatorLineColorChanged(const Vec3f & newColor);
	void supergalacticEquatorLineDisplayedChanged(const bool displayed);
	void supergalacticEquatorPartsDisplayedChanged(const bool displayed);
	void supergalacticEquatorPartsLabeledChanged(const bool displayed);
	void supergalacticEquatorLineColorChanged(const Vec3f & newColor);
	void primeVerticalLineDisplayedChanged(const bool displayed);
	void primeVerticalPartsDisplayedChanged(const bool displayed);
	void primeVerticalPartsLabeledChanged(const bool displayed);
	void primeVerticalLineColorChanged(const Vec3f & newColor);
	void currentVerticalLineDisplayedChanged(const bool displayed);
	void currentVerticalPartsDisplayedChanged(const bool displayed);
	void currentVerticalPartsLabeledChanged(const bool displayed);
	void currentVerticalLineColorChanged(const Vec3f & newColor);
	void colureLinesDisplayedChanged(const bool displayed);
	void colurePartsDisplayedChanged(const bool displayed);
	void colurePartsLabeledChanged(const bool displayed);
	void colureLinesColorChanged(const Vec3f & newColor);
	void circumpolarCirclesDisplayedChanged(const bool displayed);
	void circumpolarCirclesColorChanged(const Vec3f & newColor);
	void umbraCircleDisplayedChanged(const bool displayed);
	void umbraCircleColorChanged(const Vec3f & newColor);
	void penumbraCircleDisplayedChanged(const bool displayed);
	void penumbraCircleColorChanged(const Vec3f & newColor);
	void celestialJ2000PolesDisplayedChanged(const bool displayed);
	void celestialJ2000PolesColorChanged(const Vec3f & newColor);
	void celestialPolesDisplayedChanged(const bool displayed);
	void celestialPolesColorChanged(const Vec3f & newColor);
	void zenithNadirDisplayedChanged(const bool displayed);
	void zenithNadirColorChanged(const Vec3f & newColor);
	void eclipticJ2000PolesDisplayedChanged(const bool displayed);
	void eclipticJ2000PolesColorChanged(const Vec3f & newColor);
	void eclipticPolesDisplayedChanged(const bool displayed);
	void eclipticPolesColorChanged(const Vec3f & newColor);
	void galacticPolesDisplayedChanged(const bool displayed);
	void galacticPolesColorChanged(const Vec3f & newColor);
	void galacticCenterDisplayedChanged(const bool displayed);
	void galacticCenterColorChanged(const Vec3f & newColor);
	void supergalacticPolesDisplayedChanged(const bool displayed);
	void supergalacticPolesColorChanged(const Vec3f & newColor);
	void equinoxJ2000PointsDisplayedChanged(const bool displayed);
	void equinoxJ2000PointsColorChanged(const Vec3f & newColor);
	void equinoxPointsDisplayedChanged(const bool displayed);
	void equinoxPointsColorChanged(const Vec3f & newColor);
	void solsticeJ2000PointsDisplayedChanged(const bool displayed);
	void solsticeJ2000PointsColorChanged(const Vec3f & newColor);
	void solsticePointsDisplayedChanged(const bool displayed);
	void solsticePointsColorChanged(const Vec3f & newColor);
	void antisolarPointDisplayedChanged(const bool displayed);
	void antisolarPointColorChanged(const Vec3f & newColor);
	void umbraCenterPointDisplayedChanged(const bool displayed);
	void apexPointsDisplayedChanged(const bool displayed);
	void apexPointsColorChanged(const Vec3f & newColor);

private slots:
	//! Re-translate the labels of the great circles.
	//! Contains only calls to SkyLine::updateLabel() and SkyPoint::updateLabel().
	void updateLabels();
	//! Connect the earth shared pointer.
	//! Must be connected to SolarSystem::solarSystemDataReloaded()
	void connectSolarSystem();

	//! Connect from StelApp to reset all fonts of the grids, lines and points.
	void setFontSizeFromApp(int size);

private:
	QSharedPointer<Planet> earth;		// shortcut Earth pointer. Must be reconnected whenever solar system has been reloaded.
	bool gridlinesDisplayed;		// master switch to switch off all grids/lines. (useful for oculars plugin)
	SkyGrid * equGrid;			// Equatorial grid
	SkyGrid * equJ2000Grid;			// Equatorial J2000 grid
	SkyGrid * fixedEquatorialGrid;		// Fixed Equatorial grid (hour angle/declination)
	SkyGrid * galacticGrid;			// Galactic grid
	SkyGrid * supergalacticGrid;		// Supergalactic grid
	SkyGrid * eclGrid;			// Ecliptic of Date grid
	SkyGrid * eclJ2000Grid;			// Ecliptic J2000 grid
	SkyGrid * aziGrid;			// Azimuthal grid
	SkyLine * equatorLine;			// Celestial Equator line
	SkyLine * equatorJ2000Line;		// Celestial Equator line of J2000
	SkyLine * fixedEquatorLine;		// Fixed Celestial Equator line (hour angles)
	SkyLine * eclipticLine;			// Ecliptic line
	SkyLine * eclipticWithDateLine;		// Ecliptic line (line actually invisible!) with date partitions for the current year indicating Solar position at midnight
	SkyLine * eclipticJ2000Line;		// Ecliptic line of J2000
	SkyLine * invariablePlaneLine;		// Invariable Plane of the Solar System (WGCCRE2015 report)
	SkyLine * solarEquatorLine;		// Projected Solar equator (WGCCRE2015 report)
	SkyLine * precessionCircleN;		// Northern precession circle
	SkyLine * precessionCircleS;		// Southern precession circle
	SkyLine * meridianLine;			// Meridian line
	SkyLine * longitudeLine;		// Opposition/conjunction longitude line
	SkyLine * quadratureLine;		// Quadrature line
	SkyLine * horizonLine;			// Horizon line
	SkyLine * galacticEquatorLine;		// line depicting the Galactic equator as defined by the IAU definition of Galactic coordinates (System II, 1958)
	SkyLine * supergalacticEquatorLine;	// line depicting the Supergalactic equator
	SkyLine * primeVerticalLine;		// Prime Vertical line
	SkyLine * currentVerticalLine;		// Vertical line for azimuth of display center. Most useful if altitudes labeled.
	SkyLine * colureLine_1;			// First Colure line (0/12h)
	SkyLine * colureLine_2;			// Second Colure line (6/18h)
	SkyLine * circumpolarCircleN;		// Northern circumpolar circle
	SkyLine * circumpolarCircleS;		// Southern circumpolar circle
	SkyLine * umbraCircle;			// Umbra circle (Earth shadow in Lunar distance)
	SkyLine * penumbraCircle;		// Penumbra circle (Earth partial shadow in Lunar distance)
	SkyPoint * celestialJ2000Poles;		// Celestial poles of J2000
	SkyPoint * celestialPoles;		// Celestial poles
	SkyPoint * zenithNadir;			// Zenith and nadir
	SkyPoint * eclipticJ2000Poles;		// Ecliptic poles of J2000
	SkyPoint * eclipticPoles;		// Ecliptic poles
	SkyPoint * galacticPoles;		// Galactic poles
	SkyPoint * galacticCenter;		// Galactic center and anticenter
	SkyPoint * supergalacticPoles;		// Supergalactic poles
	SkyPoint * equinoxJ2000Points;		// Equinox points of J2000
	SkyPoint * equinoxPoints;		// Equinox points
	SkyPoint * solsticeJ2000Points;		// Solstice points of J2000
	SkyPoint * solsticePoints;		// Solstice points
	SkyPoint * antisolarPoint;		// Antisolar point
	SkyPoint * umbraCenterPoint;		// The point of the center of umbra
	SkyPoint * apexPoints;			// Apex and Antapex points, i.e. the point where the observer planet is moving to or receding from	
};

#endif // GRIDLINESMGR_HPP
