from keras.models import load_model
from keras.models import Model
import numpy as np
def getFeature(arr, connect):
	print("start")
	arr = np.array(arr, dtype = "float32")
	arr = arr.reshape(1, 64, 64, 1)
	feature = connect.predict(arr);
	# print(feature[0])
	feature_list = []
	for i in range(len(feature[0])):
		feature_list.append(feature[0][i])
	return feature_list
	
	
def getFunc():
	model = load_model('face_model_3.h5')
	connect = Model(input=model.input, output=model.get_layer(index=17).output)
	return connect