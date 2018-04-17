struct InputData
{
  float3 v1;
};

struct OutputData
{
  float mag;
};

StructuredBuffer<float3> gInput : register(t0);
RWStructuredBuffer<float> gOutput : register(u0);

//ConsumeStructuredBuffer<InputData> gInput : register(t0);
//AppendStructuredBuffer<OutputData> gOutput : register(u0);
 
[numthreads(64, 1, 1)]
void exerciseCS(int3 dispatchThreadID : SV_DispatchThreadID)
{
	int index = dispatchThreadID.x;
	//float3 v1 = gInput[index].v1;
	float3 v1 = gInput[index];
	//float3 v1 = gInput.Consume();

  /*OutputData out;
  out.mag = sqrt(v1.x * v1.x 
         + v1.y * v1.y
         + v1.z * v1.z);
         
  gOutput.Append(out);*/
	//gOutput[index].mag = 
	gOutput[index] =
    sqrt(v1.x * v1.x 
         + v1.y * v1.y
         + v1.z * v1.z);
}