// This has been adapted from the Vulkan tutorial

#include "MyProject.hpp"
#include "Disk.h"


// The uniform buffer object used in this example
struct globalUniformBufferObject {
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	//aggiunte per la luce
	alignas(16) glm::vec3 lightDir;
	alignas(16) glm::vec3 lightPos;
	alignas(16) glm::vec3 lightColor;
	alignas(16) glm::vec4 lightParams;
	alignas(16) glm::vec3 eyePos;
};
struct UniformBufferObject {
	alignas(16) glm::mat4 model;
};

// MAIN ! 
class MyProject : public BaseProject {
	protected:
	// Here you list all the Vulkan objects you need:
	
	// Descriptor Layouts [what will be passed to the shaders]
	DescriptorSetLayout DSLglobal;
	DescriptorSetLayout DSLobj;

	// Pipelines [Shader couples]
	Pipeline P1;

	// Models, textures and Descriptors (values assigned to the uniforms)
	Model M_Table;
	Texture T_Table;
	DescriptorSet DS_Table;	//instances of DSLobj

	Model M_Disk;
	Texture T_Disk;
	DescriptorSet DS_Disk;	

	Model M_Paddle;
	Texture T_Paddle1;
	DescriptorSet DS_Paddle1;	//First instance of the paddle

	Texture T_Paddle2;
	DescriptorSet DS_Paddle2;   //Second instance of the paddle
	
	Model M_Ground;
	Texture T_Ground;
	DescriptorSet DS_Ground;	

	// Instance DS global
	DescriptorSet DS_global;	//instance of DSLglobal

	globalUniformBufferObject gubo{};
	int view;


	glm::vec3 Pos_p1 = glm::vec3(-0.57f, 0.0f, 0.0f);
	glm::vec3 Pos_p2 = glm::vec3(0.57f, 0.0f, 0.0f);
	glm::vec2 startingPosDisk = glm::vec2(0.0f,0.0f);

	glm::vec2 startingSpeedDisk = glm::vec2(0.0f, 0.0f);

	float radiusDisk = 0.029;
	float radiusPaddle = 0.0705;
	float widthTable = 1.014-2*0.052;
	float lengthTable = 1.893-2*0.068;
	Disk disk = Disk::Disk(startingPosDisk, startingSpeedDisk, radiusDisk, radiusPaddle, widthTable, lengthTable);
	

	
	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 1920;
		windowHeight = 1080;
		windowTitle = "Air Hockey";
		initialBackgroundColor = {0.0f, 0.0f, 0.0f, 1.0f};
		
		// Descriptor pool sizes
		uniformBlocksInPool = 6;
		texturesInPool = 5;
		setsInPool = 6;

	}
	
	// Here you load and setup all your Vulkan objects
	void localInit() {
		// Descriptor Layouts [what will be passed to the shaders]
		DSLobj.init(this, {
					// this array contains the binding:
					// first  element : the binding number
					// second element : the time of element (buffer or texture)
					// third  element : the pipeline stage where it will be used
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
				  });
		DSLglobal.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_ALL_GRAPHICS},
			});


		// Pipelines [Shader couples]
		// The last array, is a vector of pointer to the layouts of the sets that will
		// be used in this pipeline. The first element will be set 0, and so on..
		P1.init(this, "shaders/vert.spv", "shaders/frag.spv", {&DSLglobal, &DSLobj});

		// Models, textures and Descriptors (values assigned to the uniforms)
		M_Table.init(this, "models/table.obj");
		T_Table.init(this, "textures/airhockey-background.png");
		DS_Table.init(this, &DSLobj, {
		// the second parameter, is a pointer to the Uniform Set Layout of this set
		// the last parameter is an array, with one element per binding of the set.
		// first  elmenet : the binding number
		// second element : UNIFORM or TEXTURE (an enum) depending on the type
		// third  element : only for UNIFORMs, the size of the corresponding C++ object
		// fourth element : only for TEXTUREs, the pointer to the corresponding texture object
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T_Table}
				});

		//Disk initialization

		M_Disk.init(this, "models/disk.obj");
		T_Disk.init(this, "textures/disk.png");
		DS_Disk.init(this, &DSLobj, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_Disk}
			});

		M_Paddle.init(this, "models/paddle.obj");
		T_Paddle1.init(this, "textures/paddle1.png");
		DS_Paddle1.init(this, &DSLobj, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_Paddle1}
			});

		T_Paddle2.init(this, "textures/paddle2.png");
		DS_Paddle2.init(this, &DSLobj, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_Paddle2}
			});

		M_Ground.init(this, "models/ground.obj");
		T_Ground.init(this, "textures/ground.png");
		DS_Ground.init(this, &DSLobj, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_Ground}
			});

		//DS initialization
		DS_global.init(this, &DSLglobal, {
						{0, UNIFORM, sizeof(globalUniformBufferObject), nullptr},
			});

		gubo.view = glm::lookAt(glm::vec3(-1.5f, 0.5f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		view = 0;
		gubo.proj = glm::perspective(glm::radians(45.0f),
			swapChainExtent.width / (float)swapChainExtent.height,
			0.1f, 10.0f);
		gubo.proj[1][1] *= -1;
		



		
	}

	// Here you destroy all the objects you created!		
	void localCleanup() {
		DS_Table.cleanup();
		T_Table.cleanup();
		M_Table.cleanup();

		DS_Disk.cleanup();
		M_Disk.cleanup();
		T_Disk.cleanup();

		DS_Paddle1.cleanup();
		M_Paddle.cleanup();
		T_Paddle1.cleanup();

		DS_Paddle2.cleanup();
		T_Paddle2.cleanup();

		DS_Ground.cleanup();
		T_Ground.cleanup();
		M_Ground.cleanup();

		P1.cleanup();
		DS_global.cleanup();

		DSLglobal.cleanup();
		DSLobj.cleanup();
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
				
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				P1.graphicsPipeline);

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 0, 1, &DS_global.descriptorSets[currentImage],
			0, nullptr);
				
		VkBuffer vertexBuffers[] = {M_Table.vertexBuffer};
		// property .vertexBuffer of models, contains the VkBuffer handle to its vertex buffer
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		// property .indexBuffer of models, contains the VkBuffer handle to its index buffer
		vkCmdBindIndexBuffer(commandBuffer, M_Table.indexBuffer, 0,
								VK_INDEX_TYPE_UINT32);

		// property .pipelineLayout of a pipeline contains its layout.
		// property .descriptorSets of a descriptor set contains its elements.
		vkCmdBindDescriptorSets(commandBuffer,
						VK_PIPELINE_BIND_POINT_GRAPHICS,
						P1.pipelineLayout, 1, 1, &DS_Table.descriptorSets[currentImage],
						0, nullptr);
						
		// property .indices.size() of models, contains the number of triangles * 3 of the mesh.
		vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(M_Table.indices.size()), 1, 0, 0, 0);

		//Disk buffer initialization

		VkBuffer vertexBuffers_Disk[] = { M_Disk.vertexBuffer };
		VkDeviceSize offsets_Disk[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers_Disk, offsets_Disk);
		vkCmdBindIndexBuffer(commandBuffer, M_Disk.indexBuffer, 0,
			VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &DS_Disk.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_Disk.indices.size()), 1, 0, 0, 0);

		//Paddles buffer initialization

		VkBuffer vertexBuffers_Paddle[] = { M_Paddle.vertexBuffer };
		VkDeviceSize offsets_Paddle[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers_Paddle, offsets_Paddle);
		vkCmdBindIndexBuffer(commandBuffer, M_Paddle.indexBuffer, 0,
			VK_INDEX_TYPE_UINT32);
		//Paddle1
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &DS_Paddle1.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_Paddle.indices.size()), 1, 0, 0, 0);
		//Paddle2
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &DS_Paddle2.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_Paddle.indices.size()), 1, 0, 0, 0);


		//Ground buffer initialization

		VkBuffer vertexBuffers_Ground[] = { M_Ground.vertexBuffer };
		VkDeviceSize offsets_Ground[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers_Ground, offsets_Ground);
		vkCmdBindIndexBuffer(commandBuffer, M_Ground.indexBuffer, 0,
			VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &DS_Ground.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_Ground.indices.size()), 1, 0, 0, 0);
	}


	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		
		//First part to use for translations
				
		static auto startTime = std::chrono::high_resolution_clock::now();
		static float lastTime = 0.0f;

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>
			(currentTime - startTime).count();
		float deltaT = time - lastTime;
		lastTime = time;

		static float debounce = time;

		


	//VAR FOR INPUT MANAGEMENT

		float static yaw_p1 = 0.0f, pitch_p1 = 0.0f, roll_p1;
		float static yaw_p2 = 0.0f, pitch_p2 = 0.0f, roll_p2;
		float mx_p1 = 0, my_p1 = 0, mz_p1 = 0;
		float mx_p2 = 0, my_p2 = 0, mz_p2 = 0;
		float m_change;
		float move_speed_paddles = 1.15;

	//INPUT MANAGEMENT
		//Paddle1
		if (glfwGetKey(window, GLFW_KEY_D)) {
			mx_p1 = 1;
			roll_p1 = 0.0f;
		}
		if (glfwGetKey(window, GLFW_KEY_A)) {
			mx_p1 = -1;
			roll_p1 = 180.0f;
		}
		if (glfwGetKey(window, GLFW_KEY_W)) {
			mz_p1 = 1;
			roll_p1 = 90.0f - 45.0f * mx_p1;
		}
		if (glfwGetKey(window, GLFW_KEY_S)) {
			mz_p1 = -1;
			roll_p1 = -90.0f + 45.0f * mx_p1;
		}

		//Paddle2
		if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
			mx_p2 = 1;
			roll_p2 = 0.0f;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT)) {
			mx_p2 = -1;
			roll_p2 = 180.0f;
		}
		if (glfwGetKey(window, GLFW_KEY_UP)) {
			mz_p2 = 1;
			roll_p2 = 90.0f - 45.0f * mx_p2;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN)) {
			mz_p2 = -1;
			roll_p2 = -90.0f + 45.0f * mx_p2;
		}
		
		
		UniformBufferObject ubo{};
		
			
		void* data;
	


	//VIEW CHANGING

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			if (time - debounce > 0.5) {
				view++;
				debounce = time;
				view = view % 3;
			}
		}
		if (view == 0) {
			gubo.view = glm::lookAt(glm::vec3(0.0f, 1.0f, 4.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
			gubo.eyePos = glm::vec3(0.0f, 1.0f, 1.0f);
		}
		else if (view == 1)
		{
			gubo.view = glm::lookAt(glm::vec3(-1.5f, 0.5f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));

			m_change = -mx_p1;
			mx_p1 = mz_p1;
			mz_p1 = m_change;

			m_change = -mx_p2;
			mx_p2 = mz_p2;
			mz_p2 = m_change;

			gubo.eyePos = glm::vec3(-1.5f, 0.5f, 0.0f);

		}
		else if (view == 2)
		{
			gubo.view = glm::lookAt(glm::vec3(1.5f, 0.5f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));

			m_change = mx_p1;
			mx_p1 = -mz_p1;
			mz_p1 = m_change;

			m_change = mx_p2;
			mx_p2 = -mz_p2;
			mz_p2 = m_change;

			gubo.eyePos = glm::vec3(1.5f, 0.5f, 0.0f);
		}

		

		//Disk
		disk.updateDisk(deltaT, glm::vec2(Pos_p1.x, Pos_p1.z), glm::vec2(Pos_p2.x, Pos_p2.z), glm::vec2(mx_p1, -mz_p1), glm::vec2(mx_p2, -mz_p2));

		
		//POSITION UPDATE FOR PADDLES
		
		if(Pos_p1.x + 0.0705 + mx_p1 * deltaT / move_speed_paddles <= 0.0 &&
		   Pos_p1.x - 0.0705 + mx_p1 * deltaT / move_speed_paddles >= -0.9465+0.068)
		Pos_p1.x += mx_p1 * deltaT / move_speed_paddles;
		if (Pos_p1.z + 0.0705 - mz_p1 * deltaT / move_speed_paddles <=  0.507 - 0.052 &&
			Pos_p1.z - 0.0705 - mz_p1 * deltaT / move_speed_paddles >= -0.507 + 0.052)
		Pos_p1.z -= mz_p1 * deltaT / move_speed_paddles;

		
		if (Pos_p2.x - 0.0705 + mx_p2 * deltaT / move_speed_paddles >= 0.0 &&
			Pos_p2.x + 0.0705 + mx_p2 * deltaT / move_speed_paddles <= 0.9465-0.068)
		Pos_p2.x += mx_p2 * deltaT / move_speed_paddles;
		if (Pos_p2.z + 0.0705 - mz_p2 * deltaT / move_speed_paddles <= 0.507 - 0.052 &&
			Pos_p2.z - 0.0705 - mz_p2 * deltaT / move_speed_paddles >= -0.507 + 0.052)
		Pos_p2.z -= mz_p2 * deltaT / move_speed_paddles;

		//Adding part for illumination in shader

		gubo.lightColor = glm::vec3(1.0f, 1.0f, 1.0f)*0.3f;
		gubo.lightDir = glm::vec3(cos(glm::radians(90.0f)) * cos(glm::radians(0.0f)), sin(glm::radians(90.0f)), cos(glm::radians(90.0f)) * sin(glm::radians(0.0f)));
		gubo.lightPos = glm::vec3(0.0, 1.0, 0.0);
		gubo.lightParams = glm::vec4(
			cos(glm::radians(10.5f)), cos(glm::radians(25.0f)), 2.0f, 1.8f
		);
		
		
		//TO SEE

		vkMapMemory(device, DS_global.uniformBuffersMemory[0][currentImage], 0,
			sizeof(gubo), 0, &data);
		memcpy(data, &gubo, sizeof(gubo));
		vkUnmapMemory(device, DS_global.uniformBuffersMemory[0][currentImage]);
		

		//For the table
		ubo.model = glm::mat4(1.0f);
		
		//Previous table rotaded
		/*ubo.model = glm::rotate(glm::mat4(1.0f),
			glm::radians(45.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));*/

		// Here is where you actually update your uniforms
		vkMapMemory(device, DS_Table.uniformBuffersMemory[0][currentImage], 0,
							sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_Table.uniformBuffersMemory[0][currentImage]);


		//For the disk
		ubo.model = glm::translate(glm::mat4(1.0f),glm::vec3(disk.getPos().x, 0.0f, disk.getPos().y));

		// Here is where you actually update your uniforms
		vkMapMemory(device, DS_Disk.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_Disk.uniformBuffersMemory[0][currentImage]);

		//For the first paddle
		ubo.model = glm::translate(glm::mat4(1.0), glm::vec3(Pos_p1.x, Pos_p1.y, Pos_p1.z)) *
			glm::rotate(glm::mat4(1.0), glm::radians(roll_p1), glm::vec3(0, 1, 0)) *
			glm::rotate(glm::mat4(1.0), glm::radians(pitch_p1), glm::vec3(1, 0, 0)) *
			glm::rotate(glm::mat4(1.0), glm::radians(yaw_p1), glm::vec3(0, 0, 1)) *
			glm::scale(glm::mat4(1.0), glm::vec3(1, 1, 1));

		// Here is where you actually update your uniforms
		vkMapMemory(device, DS_Paddle1.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_Paddle1.uniformBuffersMemory[0][currentImage]);

		//For the second paddle
		ubo.model = glm::translate(glm::mat4(1.0), glm::vec3(Pos_p2.x, Pos_p2.y, Pos_p2.z)) *
			glm::rotate(glm::mat4(1.0), glm::radians(roll_p2), glm::vec3(0, 1, 0)) *
			glm::rotate(glm::mat4(1.0), glm::radians(pitch_p2), glm::vec3(1, 0, 0)) *
			glm::rotate(glm::mat4(1.0), glm::radians(yaw_p2), glm::vec3(0, 0, 1)) *
			glm::scale(glm::mat4(1.0), glm::vec3(1, 1, 1));;

		// Here is where you actually update your uniforms
		vkMapMemory(device, DS_Paddle2.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_Paddle2.uniformBuffersMemory[0][currentImage]);

		//For the ground
		ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.7f, 0.0f));

		vkMapMemory(device, DS_Ground.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_Ground.uniformBuffersMemory[0][currentImage]);
	}	
};

// This is the main: probably you do not need to touch this!
int main() {
    MyProject app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}