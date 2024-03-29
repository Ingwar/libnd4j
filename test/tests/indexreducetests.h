//
// Created by agibsoncccc on 1/5/16.
//

#ifndef NATIVEOPERATIONS_INDEXREDUCETESTS_H_H
#define NATIVEOPERATIONS_INDEXREDUCETESTS_H_H
#include <array.h>
#include "testhelpers.h"
#include <indexreduce.h>

TEST_GROUP(IndexReduce) {

	static int output_method(const char* output, ...) {
		va_list arguments;
		va_start(arguments, output);
		va_end(arguments);
		return 1;
	}
	void setup() {

	}
	void teardown() {
	}
};



template <typename T>
class IndexReduceTest : public BaseTest<T> {
public:
	virtual ~IndexReduceTest() {
		freeOpAndOpFactory();
	}
	IndexReduceTest(int rank,int opNum,Data<T> *data,int extraParamsLength)
	:  BaseTest<T>(rank,opNum,data,extraParamsLength){
		createOperationAndOpFactory();
	}
	void freeOpAndOpFactory() {
		delete opFactory;
		delete reduce;
	}

	virtual void createOperationAndOpFactory() {
		opFactory = new functions::indexreduce::IndexReduceOpFactory<T>();
		reduce = opFactory->create(this->opNum);
	}

	virtual void execCpuKernel() override {
		int *xShapeBuff = shapeBuffer(this->baseData->xShape,this->baseData->rank);
		int *resultShapeBuff = shapeBuffer(this->baseData->resultShape,this->baseData->resultRank);
		reduce->exec(this->data->data,xShapeBuff,
				this->baseData->extraParams,this->result->data,
				resultShapeInfo,this->baseData->dimension,this->baseData->dimensionLength);
		free(xShapeBuff);
		free(resultShapeBuff);

	}

protected:
	functions::reduce::ReduceOpFactory<T> *opFactory;
	functions::reduce::ReduceFunction<T> *reduce;
};

class DoubleIndexReduceTest : public  ReduceTest<double> {
public:
	virtual ~DoubleIndexReduceTest() {}
	DoubleIndexReduceTest() {}
	DoubleIndexReduceTest(int rank,int opNum,Data<double> *data,int extraParamsLength)
	:  ReduceTest<double>(rank,opNum,data,extraParamsLength){
	}
	virtual void executeCudaKernel() override {
#ifdef __CUDACC__
		nd4j::buffer::Buffer<int> *gpuInfo = this->gpuInformationBuffer();
		nd4j::buffer::Buffer<int> *dimensionBuffer = nd4j::buffer::createBuffer(this->baseData->dimension,this->baseData->dimensionLength);
		nd4j::buffer::Buffer<int> *xShapeBuff = shapeIntBuffer(this->rank,this->shape);
		nd4j::buffer::Buffer<int> *resultShapeBuff = shapeIntBuffer(this->result->rank,this->result->shape->data);

		indexReduceDouble<<<this->blockSize,this->gridSize,this->sMemSize>>>(
				this->opNum,
				this->length,
				this->data->data->gData,
				xShapeBuff->gData,
				extraParamsBuff->gData,
				this->result->data->gData,
				resultShapeBuff->gData,
				gpuInfo->gData,
				dimensionBuffer->gData,
				this->baseData->dimensionLength,
				1
		);

		nd4j::buffer::freeBuffer(&gpuInfo);
		nd4j::buffer::freeBuffer(&dimensionBuffer);
		nd4j::buffer::freeBuffer(&xShapeBuff);
		nd4j::buffer::freeBuffer(&resultShapeBuff);
#endif

	}
};


class FloatIndexReduceTest : public ReduceTest<float> {
public:
	FloatIndexReduceTest() {}
	FloatIndexReduceTest(int rank,int opNum,Data<float> *data,int extraParamsLength)
	:  ReduceTest<float>(rank,opNum,data,extraParamsLength){
	}
	virtual void executeCudaKernel() override {
#ifdef __CUDACC__
		nd4j::buffer::Buffer<int> *gpuInfo = this->gpuInformationBuffer();
		nd4j::buffer::Buffer<int> *dimensionBuffer = nd4j::buffer::createBuffer(this->baseData->dimension,this->baseData->dimensionLength);
		nd4j::buffer::Buffer<int> *xShapeBuff = shapeIntBuffer(this->rank,this->shape);
		nd4j::buffer::Buffer<int> *resultShapeBuff = shapeIntBuffer(this->result->rank,this->result->shape->data);

		indexReduceFloat<<<this->blockSize,this->gridSize,this->sMemSize>>>(
				this->opNum,
				this->length,
				this->data->data->gData,
				xShapeBuff->gData,
				extraParamsBuff->gData,
				this->result->data->gData,
				resultShapeBuff->gData,
				gpuInfo->gData,
				dimensionBuffer->gData,
				this->baseData->dimensionLength,
				1
		);

		nd4j::buffer::freeBuffer(&gpuInfo);
		nd4j::buffer::freeBuffer(&dimensionBuffer);
		nd4j::buffer::freeBuffer(&xShapeBuff);
		nd4j::buffer::freeBuffer(&resultShapeBuff);
#endif
	}
};



TEST(IndexReduce, IMax) {
	functions::indexreduce::IndexReduceOpFactory<double> *opFactory5 =
			new functions::indexreduce::IndexReduceOpFactory<double>();
	functions::indexreduce::IndexReduce<double> *sum = opFactory5->getOp(0);
	CHECK(sum != NULL);
	int length = 4;
	double *data = (double *) malloc(sizeof(double) * length);
	for (int i = 0; i < length; i++) {
		data[i] = i + 1;
	}


	int *resultShapeInfo = shape::createScalarShapeInfo();

	shape::ShapeInformation *shapeInfo = (shape::ShapeInformation *) malloc(
			sizeof(shape::ShapeInformation));
	int rank = 2;
	int *shape = (int *) malloc(sizeof(int) * rank);
	shape[0] = 1;
	shape[1] = length;
	int *stride = shape::calcStrides(shape, rank);
	shapeInfo->shape = shape;
	shapeInfo->stride = stride;
	shapeInfo->offset = 0;
	shapeInfo->elementWiseStride = 1;

	int dimensionLength = 1;
	int *dimension = (int *) malloc(sizeof(int) * dimensionLength);
	dimension[0] =  shape::MAX_DIMENSION;


	int *shapeBuffer = shape::toShapeBuffer(shapeInfo);
	double *extraParams = (double *) malloc(sizeof(double));
	extraParams[0] = 0.0;

	double *result = (double *) malloc(2 * sizeof(double));
	result[0] = 0.0;
	sum->exec(data, shapeBuffer, extraParams, result, resultShapeInfo);
	double comp = result[0];
	CHECK(3.0 == comp);
	nd4j::array::NDArray<double> *dataArr = nd4j::array::NDArrays<double>::createFromShapeInfo(shapeInfo,0.0);
	dataArr->data->data = data;
	nd4j::buffer::Buffer<int> *shapeBufferBuff = nd4j::buffer::createBuffer(shapeBuffer,shape::shapeInfoLength(shapeInfo->rank));
	nd4j::buffer::Buffer<int> *dimensionBuff = nd4j::buffer::createBuffer(dimension,1);
	nd4j::buffer::Buffer<double> *extraParamsBuff = nd4j::buffer::createBuffer(extraParams,1);
    nd4j::buffer::Buffer<int> *resultShapeInfoBuff = nd4j::buffer::createBuffer<int>(resultShapeInfo,shape::shapeInfoLength(shape::rank(resultShapeInfo)));
    nd4j::buffer::Buffer<double> *resultDataBuffer = nd4j::buffer::createBuffer<double>(result,2);

#ifdef __CUDACC__
    setupIndexReduceFactories();
    nd4j::array::NDArrays<double>::allocateNDArrayOnGpu(&dataArr);
	nd4j::buffer::copyDataToGpu<int>(&shapeBufferBuff);
	nd4j::buffer::copyDataToGpu<double>(&extraParamsBuff);
	nd4j::buffer::copyDataToGpu<int>(&dimensionBuff);
	nd4j::buffer::copyDataToGpu<int>(&resultShapeInfoBuff);
	nd4j::buffer::copyDataToGpu<double>(&resultDataBuffer);
	int blockSize = 500;
	int gridSize = 256;
	int sMemSize = 20000;



	int *gpuInformation = (int *) malloc(sizeof(int) * 4);
	gpuInformation[0] = blockSize;
	gpuInformation[1] = gridSize;
	gpuInformation[2] = sMemSize;
	gpuInformation[3] = 49152;
	nd4j::buffer::Buffer<int> *gpuInfoBuff = nd4j::buffer::createBuffer<int>(gpuInformation,4);
	nd4j::buffer::copyDataToGpu(&gpuInfoBuff);

	indexReduceDouble<<<blockSize,gridSize,sMemSize>>>(
			0,
			length,
			dataArr->data->gData,
			shapeBufferBuff->gData,
			extraParamsBuff->gData,
			resultDataBuffer->gData,
			resultShapeInfoBuff->gData,
			gpuInfoBuff->gData,
			dimensionBuff->gData,
			dimensionLength
			,1);
	checkCudaErrors(cudaDeviceSynchronize());
	nd4j::buffer::copyDataFromGpu(&resultDataBuffer);
	CHECK(result[0] == 3.0);
	nd4j::buffer::freeBuffer<int>(&gpuInfoBuff);

#endif

	nd4j::buffer::freeBuffer<double>(&resultDataBuffer);
	nd4j::array::NDArrays<double>::freeNDArrayOnGpuAndCpu(&dataArr);
	nd4j::buffer::freeBuffer<int>(&shapeBufferBuff);
	free(extraParams);
	delete sum;
	delete opFactory5;

}


TEST(IndexReduce,DimensionIMax) {
	functions::indexreduce::IndexReduceOpFactory<double> *opFactory5 =
			new functions::indexreduce::IndexReduceOpFactory<double>();
	functions::indexreduce::IndexReduce<double> *sum = opFactory5->getOp(0);
	CHECK(sum != NULL);
	int length = 4;
	double *data = (double *) malloc(sizeof(double) * length);
	for (int i = 0; i < length; i++) {
		data[i] = i + 1;
	}
	int *resultShapeInfo = shape::createScalarShapeInfo();

	shape::ShapeInformation *shapeInfo = (shape::ShapeInformation *) malloc(
			sizeof(shape::ShapeInformation));
	int rank = 2;
	int *shape = (int *) malloc(sizeof(int) * rank);
	shape[0] = 2;
	shape[1] = 2;
	int *stride = shape::calcStrides(shape, rank);
	shapeInfo->shape = shape;
	shapeInfo->stride = stride;
	shapeInfo->offset = 0;
	shapeInfo->elementWiseStride = 1;

	int *shapeBuffer = shape::toShapeBuffer(shapeInfo);
	double *extraParams = (double *) malloc(sizeof(double));
	extraParams[0] = 0.0;

	int resultLength = 2;
	double *result = (double *) malloc(resultLength * sizeof(double));
	for (int i = 0; i < resultLength; i++)
		result[i] = 0.0;
	int dimensionLength = 1;
	int *dimension = (int *) malloc(dimensionLength * sizeof(int));
	dimension[0] = 1;

	sum->exec(data, shapeBuffer, extraParams, result, resultShapeInfo,
			dimension, dimensionLength);
	double *comp = (double *) malloc(sizeof(double) * resultLength);
	for (int i = 0; i < resultLength; i++) {
		comp[i] = 1.0;
	}

	CHECK(arrsEquals(2, comp, result));
	free(comp);
	free(extraParams);
	free(dimension);
	free(shapeBuffer);
	free(shapeInfo);
	delete sum;
	free(data);
	delete opFactory5;
}

#endif //NATIVEOPERATIONS_INDEXREDUCETESTS_H_H
