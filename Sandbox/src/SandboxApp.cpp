#include<Opal.h>
class Stains : public Opal::Application
{
public:
	Stains()
	{

	}
	~Stains()
	{

	}

};

Opal::Application* Opal::CreateApplication()
{
	return new Stains();
}