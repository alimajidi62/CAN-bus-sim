export module parentModule;
export class Interface
{
public:
	virtual ~Interface() = default;
	virtual int addMudule(int a, int b) = 0;
	virtual void Fun() = 0;
};