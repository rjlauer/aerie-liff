// declaring some 'data'
// must inherit from Baggable
class TestData : public Baggable{
};

// these typedefs are just for convenience sake
typedef boost::shared_ptr<TestData> TestDataPtr;
typedef boost::shared_ptr<const TestData> TestDataConstPtr;

// exhibiting usage
Bag bag;

TestDataPtr testdata = make_shared<TestData>();
bag.Put("testdata",testdata);

// Getting the data out by shared ptr.  Returns an empty pointer if the 
//object type mismatches or if the object doesn't exist
TestDataConstPtr testdata_out = bag.Get<TestDataConstPtr>("testdata");

// Getting object by reference.  Throws bag_exception if the object type
// doesn't match or if the object doesn't exist.
const TestData& testdata_out_ref = bag.Get<TestData>("testdata");

// operator<< is defined for some easy pretty-printing
cout<<bag<<endl;





