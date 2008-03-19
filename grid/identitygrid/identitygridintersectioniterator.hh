// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifndef DUNE_IDENTITYGRID_INTERSECTIONITERATOR_HH
#define DUNE_IDENTITYGRID_INTERSECTIONITERATOR_HH

/** \file
 * \brief The IdentityGridLeafIntersectionIterator and IdentityGridLevelIntersectionIterator classes
 */

namespace Dune {

  /** \brief Iterator over all element neighbors
   * \ingroup IdentityGrid
   * Mesh entities of codimension 0 ("elements") allow to visit all neighbors, where
   * a neighbor is an entity of codimension 0 which has a common entity of codimension 1
   * These neighbors are accessed via a IntersectionIterator. This allows the implement
   * non-matching meshes. The number of neighbors may be different from the number
   * of an element!
   */
  template<class GridImp>
  class IdentityGridLeafIntersectionIterator :
    public IntersectionIteratorDefaultImplementation <GridImp,IdentityGridLeafIntersectionIterator>
  {

    enum {dim=GridImp::dimension};

    enum {dimworld=GridImp::dimensionworld};

    // The type used to store coordinates
    typedef typename GridImp::ctype ctype;

    typedef typename GridImp::HostGridType::template Codim<0>::Entity::LeafIntersectionIterator HostLeafIntersectionIterator;

  public:

    typedef typename GridImp::template Codim<0>::EntityPointer EntityPointer;
    typedef typename GridImp::template Codim<1>::Geometry Geometry;
    typedef typename GridImp::template Codim<1>::LocalGeometry LocalGeometry;
    typedef typename GridImp::template Codim<0>::Entity Entity;

    IdentityGridLeafIntersectionIterator(const GridImp* subGrid,
                                         const HostLeafIntersectionIterator& hostIterator)
      : selfLocal_(NULL), neighborLocal_(NULL), intersectionGlobal_(NULL),
        subGrid_(subGrid),
        hostIterator_(hostIterator)
    {}

    //! The Destructor
    ~IdentityGridLeafIntersectionIterator() {};

    //! equality
    bool equals(const IdentityGridLeafIntersectionIterator<GridImp>& other) const {
      return hostIterator_ == other.hostIterator_;
    }


    //! prefix increment
    void increment() {
      ++hostIterator_;

      // Delete intersection geometry objects, if present
      if (intersectionGlobal_ != NULL) {
        delete intersectionGlobal_;
        intersectionGlobal_ = NULL;
      }

      if (selfLocal_ != NULL) {
        delete selfLocal_;
        selfLocal_ = NULL;
      }

      if (neighborLocal_ != NULL) {
        delete neighborLocal_;
        neighborLocal_ = NULL;
      }
    }


    //! return EntityPointer to the Entity on the inside of this intersection
    //! (that is the Entity where we started this Iterator)
    EntityPointer inside() const {
      return IdentityGridEntityPointer<0,GridImp> (subGrid_, hostIterator_.inside());
    }


    //! return EntityPointer to the Entity on the outside of this intersection
    //! (that is the neighboring Entity)
    EntityPointer outside() const {
      return IdentityGridEntityPointer<0,GridImp> (subGrid_, hostIterator_.outside());
    }


    //! return true if intersection is with boundary.
    bool boundary () const {
      return hostIterator_.boundary();
    }


    //! return true if across the edge an neighbor on this level exists
    bool neighbor () const {
      return hostIterator_.neighbor();
    }


    //! return information about the Boundary
    int boundaryId () const {
      return hostIterator_.boundaryId();
    }


    //! intersection of codimension 1 of this neighbor with element where
    //! iteration started.
    //! Here returned element is in LOCAL coordinates of the element
    //! where iteration started.
    const LocalGeometry& intersectionSelfLocal () const {
      if (selfLocal_ == NULL)
        selfLocal_ = new IdentityGridMakeableGeometry<dim-1,dim,GridImp>(hostIterator_.intersectionSelfLocal());

      return *selfLocal_;
    }

    //! intersection of codimension 1 of this neighbor with element where iteration started.
    //! Here returned element is in LOCAL coordinates of neighbor
    const LocalGeometry& intersectionNeighborLocal () const {
      if (neighborLocal_ == NULL)
        neighborLocal_ = new IdentityGridMakeableGeometry<dim-1,dim,GridImp>(hostIterator_.intersectionNeighborLocal());

      return *neighborLocal_;
    }

    //! intersection of codimension 1 of this neighbor with element where iteration started.
    //! Here returned element is in GLOBAL coordinates of the element where iteration started.
    const Geometry& intersectionGlobal () const {
      if (intersectionGlobal_ == NULL)
        intersectionGlobal_ = new IdentityGridMakeableGeometry<dim-1,dimworld,GridImp>(hostIterator_.intersectionGlobal());

      return *intersectionGlobal_;
    }


    //! local number of codim 1 entity in self where intersection is contained in
    int numberInSelf () const {
      return hostIterator_.numberInSelf();
    }


    //! local number of codim 1 entity in neighbor where intersection is contained
    int numberInNeighbor () const {
      return hostIterator_.numberInNeighbor();
    }


    //! return outer normal, this should be dependent on local
    //! coordinates for higher order boundary
    const FieldVector<ctype, GridImp::dimensionworld>& outerNormal (const FieldVector<ctype, GridImp::dimension-1>& local) const {
      /** \todo Why do I get 'returning reference to temporary' when not doing this copying? */
      outerNormal_ = hostIterator_.outerNormal(local);
      return outerNormal_;
    }


  private:
    //**********************************************************
    //  private methods
    //**********************************************************

    //! vector storing the outer normal
    mutable FieldVector<typename GridImp::ctype, dimworld> outerNormal_;

    //! pointer to element holding the selfLocal and selfGlobal information.
    //! This element is created on demand.
    mutable IdentityGridMakeableGeometry<dim-1,dim,GridImp>* selfLocal_;
    mutable IdentityGridMakeableGeometry<dim-1,dim,GridImp>* neighborLocal_;

    //! pointer to element holding the neighbor_global and neighbor_local
    //! information.
    mutable IdentityGridMakeableGeometry<dim-1,dimworld,GridImp>* intersectionGlobal_;

    const GridImp* subGrid_;

    HostLeafIntersectionIterator hostIterator_;
  };




  //! \todo Please doc me !
  template<class GridImp>
  class IdentityGridLevelIntersectionIterator :
    public IntersectionIteratorDefaultImplementation <GridImp,IdentityGridLevelIntersectionIterator>
  {

    enum {dim=GridImp::dimension};

    enum {dimworld=GridImp::dimensionworld};

    // The type used to store coordinates
    typedef typename GridImp::ctype ctype;

    typedef typename GridImp::HostGridType::template Codim<0>::Entity::LevelIntersectionIterator HostLevelIntersectionIterator;

  public:

    typedef typename GridImp::template Codim<0>::EntityPointer EntityPointer;
    typedef typename GridImp::template Codim<1>::Geometry Geometry;
    typedef typename GridImp::template Codim<1>::LocalGeometry LocalGeometry;
    typedef typename GridImp::template Codim<0>::Entity Entity;

    IdentityGridLevelIntersectionIterator(const GridImp* subGrid,
                                          const HostLevelIntersectionIterator& hostIterator)
      : selfLocal_(NULL), neighborLocal_(NULL), intersectionGlobal_(NULL),
        subGrid_(subGrid), hostIterator_(hostIterator)
    {}

    //! equality
    bool equals(const IdentityGridLevelIntersectionIterator<GridImp>& other) const {
      return hostIterator_ == other.hostIterator_;
    }


    //! prefix increment
    void increment() {
      ++hostIterator_;

      // Delete intersection geometry objects, if present
      if (intersectionGlobal_ != NULL) {
        delete intersectionGlobal_;
        intersectionGlobal_ = NULL;
      }

      if (selfLocal_ != NULL) {
        delete selfLocal_;
        selfLocal_ = NULL;
      }

      if (neighborLocal_ != NULL) {
        delete neighborLocal_;
        neighborLocal_ = NULL;
      }

    }


    //! return EntityPointer to the Entity on the inside of this intersection
    //! (that is the Entity where we started this Iterator)
    EntityPointer inside() const {
      return IdentityGridEntityPointer<0,GridImp> (subGrid_, hostIterator_.inside());
    }


    //! return EntityPointer to the Entity on the outside of this intersection
    //! (that is the neighboring Entity)
    EntityPointer outside() const {
      return IdentityGridEntityPointer<0,GridImp> (subGrid_, hostIterator_.outside());
    }


    /** \brief return true if intersection is with boundary.
     */
    bool boundary () const {
      return hostIterator_.boundary();
    }


    //! return true if across the edge an neighbor on this level exists
    bool neighbor () const {
      return hostIterator_.neighbor();
    }


    //! return information about the Boundary
    int boundaryId () const {
      return hostIterator_.boundaryId();
    }


    //! intersection of codimension 1 of this neighbor with element where
    //! iteration started.
    //! Here returned element is in LOCAL coordinates of the element
    //! where iteration started.
    const LocalGeometry& intersectionSelfLocal () const {
      if (selfLocal_ == NULL)
        selfLocal_ = new IdentityGridMakeableGeometry<dim-1,dim,GridImp>(hostIterator_.intersectionSelfLocal());

      return *selfLocal_;
    }

    //! intersection of codimension 1 of this neighbor with element where iteration started.
    //! Here returned element is in LOCAL coordinates of neighbor
    const LocalGeometry& intersectionNeighborLocal () const {
      if (neighborLocal_ == NULL)
        neighborLocal_ = new IdentityGridMakeableGeometry<dim-1,dim,GridImp>(hostIterator_.intersectionNeighborLocal());

      return *neighborLocal_;
    }

    //! intersection of codimension 1 of this neighbor with element where iteration started.
    //! Here returned element is in GLOBAL coordinates of the element where iteration started.
    const Geometry& intersectionGlobal () const {
      if (intersectionGlobal_ == NULL)
        intersectionGlobal_ = new IdentityGridMakeableGeometry<dim-1,dimworld,GridImp>(hostIterator_.intersectionGlobal());

      return *intersectionGlobal_;
    }


    //! local number of codim 1 entity in self where intersection is contained in
    int numberInSelf () const {
      return hostIterator_.numberInSelf();
    }


    //! local number of codim 1 entity in neighbor where intersection is contained
    int numberInNeighbor () const {
      return hostIterator_.numberInNeighbor();
    }


    //! return outer normal, this should be dependent on local
    //! coordinates for higher order boundary
    const FieldVector<ctype, dimworld>& outerNormal (const FieldVector<ctype, dim-1>& local) const {
      /** \todo Why do I get 'returning reference to temporary' when not doing this copying? */
      outerNormal_ = hostIterator_.outerNormal(local);
      return outerNormal_;
    }

  private:

    //! vector storing the outer normal
    mutable FieldVector<typename GridImp::ctype, dimworld> outerNormal_;

    //! pointer to element holding the selfLocal and selfGlobal information.
    //! This element is created on demand.
    mutable IdentityGridMakeableGeometry<dim-1,dim,GridImp>* selfLocal_;
    mutable IdentityGridMakeableGeometry<dim-1,dim,GridImp>* neighborLocal_;

    //! pointer to element holding the neighbor_global and neighbor_local
    //! information.
    mutable IdentityGridMakeableGeometry<dim-1,dimworld,GridImp>* intersectionGlobal_;

    const GridImp* subGrid_;

    HostLevelIntersectionIterator hostIterator_;

  };


}  // namespace Dune

#endif
