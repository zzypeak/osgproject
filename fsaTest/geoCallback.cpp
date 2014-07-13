
class TrailerCallback : public osg::NodeCallback
{
public:
    TrailerCallback( osg::Geometry* ribbon) : _ribbon(ribbon){}
    
    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
    {
        osg::MatrixTransform* trans = static_cast<osg::MatrixTransform*>(node);
						//std::cout<<trans->getMatrix().getTrans().x()<<"] ["
						//<<trans->getMatrix().getTrans().y()<<"] ["
						//<<trans->getMatrix().getTrans().z()<<"] "
						//<<std::endl;

        if ( trans && _ribbon.valid() )
        {
            osg::Matrix matrix = trans->getMatrix();
            osg::Vec3Array* vertices = static_cast<osg::Vec3Array*>( _ribbon->getVertexArray() );
            osg::DrawArrays* da = static_cast<osg::DrawArrays*>( _ribbon->getPrimitiveSet(0) );

			static bool flag = false;
			static osg::Vec3d _previous =  osg::Vec3d(0,0,0);
			
			if(_previous != trans->getMatrix().getTrans())
			{
				if(flag) {vertices->clear(); flag = false;}
				vertices->push_back(trans->getMatrix().getTrans());
				_previous = trans->getMatrix().getTrans();
			}
			else
			{
				flag = true;
			}
			da->setCount( vertices->size() );
			da->dirty();
            vertices->dirty();
            
            _ribbon->dirtyBound();
        }
        traverse( node, nv );
    }
    
protected:
    osg::observer_ptr<osg::Geometry> _ribbon;
};