# TensorFlow and tf.keras
import tensorflow as tf
import time

# Helper libraries
import numpy as np

print('TensorFlow version {}'.format(tf.__version__))

tflite_mnist_model = 'mnist.tflite'

interpreter = tf.lite.Interpreter(model_path=tflite_mnist_model)
interpreter.allocate_tensors()

# Load digit from the file that created above
digit = np.loadtxt('digit.txt')
# Reshape
loaded_digit = digit.reshape(28,28)
loaded_digit = np.expand_dims(loaded_digit, axis=0)
loaded_digit = np.expand_dims(loaded_digit, axis=3)
loaded_digit.shape

loaded_digit = loaded_digit.astype('float32')
input_details = interpreter.get_input_details()
interpreter.set_tensor(input_details[0]['index'], loaded_digit)

n_runs = 1000
startTime = time.time()
for i in range(0, n_runs):
    interpreter.invoke()
endTime = time.time()

output_details = interpreter.get_output_details()
output_data = interpreter.get_tensor(output_details[0]['index'])
print('Prediction results:', output_data)
print('Predicted value:', np.argmax(output_data))

print('----------------------')
print('Total elapsed time: %f ms' % ((endTime - startTime)*1000/n_runs))