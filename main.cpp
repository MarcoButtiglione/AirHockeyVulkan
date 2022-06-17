#include "MyProject.hpp"
#include "Disk.h"
#include "Paddle.h"

struct globalUniformBufferObject {
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::vec3 lightDir;
	alignas(16) glm::vec3 lightPos;
	alignas(16) glm::vec3 lightColor;
	alignas(16) glm::vec4 lightParams;
	alignas(16) float gamma;
	alignas(16) glm::vec3 switchLight;
	alignas(16) glm::vec3 eyePos;
};

struct UniformBufferObject {
	alignas(16) glm::mat4 model;
};

class MyProject : public BaseProject {
	protected:

	// Descriptor Layouts [what will be passed to the shaders]
	DescriptorSetLayout DSLglobal;
	DescriptorSetLayout DSLobj;

	// Pipelines
	Pipeline P1;

	// Models, textures and Descriptors (values assigned to the uniforms)
	//Table
	Model M_Table;
	Texture T_Table;
	DescriptorSet DS_Table;
	//Disk
	Model M_Disk;
	Texture T_Disk;
	DescriptorSet DS_Disk;
	//Paddle red and blue (Multiple Descriptor Sets, single model but multiple objects)
	Model M_Paddle;
	Texture T_Paddle1;
	DescriptorSet DS_Paddle1;
	Texture T_Paddle2;
	DescriptorSet DS_Paddle2;
	//Ground
	Model M_Ground;
	Texture T_Ground;
	DescriptorSet DS_Ground;
	//Point counter
	Model M_PointCounter;
	Texture T_PointCounter;
	DescriptorSet DS_PointCounter1;
	DescriptorSet DS_PointCounter2;
	//Point
	Model M_Point;
	DescriptorSet DS_PointRed1;
	DescriptorSet DS_PointRed2;
	DescriptorSet DS_PointRed3;
	DescriptorSet DS_PointRed4;
	DescriptorSet DS_PointRed5;
	DescriptorSet DS_PointBlue1;
	DescriptorSet DS_PointBlue2;
	DescriptorSet DS_PointBlue3;
	DescriptorSet DS_PointBlue4;
	DescriptorSet DS_PointBlue5;

	//Starting interface
	Model M_Menu;
	Texture T_Menu;
	DescriptorSet DS_Menu;

	// Instance DS global
	DescriptorSet DS_global;

	globalUniformBufferObject gubo{};

	//Game variables

	//Model dimensions
	float radiusDisk = 0.029;
	float radiusPaddle = 0.0705;
	float widthTable = 1.014 - 2 * 0.052;
	float lengthTable = 1.893 - 2 * 0.068;

	//Types of camera angle
	int view;

	//Initialization of disk and paddles
	glm::vec3 Pos_p1 = glm::vec3(-0.57f, 0.0f, 0.0f);
	glm::vec3 Pos_p2 = glm::vec3(0.57f, 0.0f, 0.0f);
	glm::vec2 startingPosDisk = glm::vec2(0.0f,0.0f);
	glm::vec2 startingSpeedDisk = glm::vec2(0.0f, 0.0f);
	Paddle paddle1 = Paddle::Paddle(Pos_p1, radiusDisk, radiusPaddle, widthTable, lengthTable, 1.15);
	Paddle paddle2 = Paddle::Paddle(Pos_p2, radiusDisk, radiusPaddle, widthTable, lengthTable, 1.15);
	Disk disk = Disk::Disk(startingPosDisk, startingSpeedDisk, radiusDisk, radiusPaddle, widthTable, lengthTable);

	//Score
	int scoreRed = 0;
	int scoreBlue = 0;

	//Initialization of spotlights
	glm::vec3 switchLight = glm::vec3(1.0f, 1.0f, 1.0f);
	float radiusSpotLight = 40.0f;


	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 1920;
		windowHeight = 1080;
		windowTitle = "Air Hockey";
		initialBackgroundColor = {0.0f, 0.0f, 0.0f, 1.0f};

		// Descriptor pool sizes
		uniformBlocksInPool = 19;
		texturesInPool =18;
		setsInPool = 19;

	}

	// Here you load and setup all your Vulkan objects
	void localInit() {
		// Descriptor Layouts [what will be passed to the shaders]
		DSLobj.init(this, {
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

		// Models, textures and Descriptors

		//Table
		M_Table.init(this, "models/table.obj");
		T_Table.init(this, "textures/airhockey-background.png");
		DS_Table.init(this, &DSLobj, {
		// first  elmenet : the binding number
		// second element : UNIFORM or TEXTURE (an enum) depending on the type
		// third  element : only for UNIFORMs, the size of the corresponding C++ object
		// fourth element : only for TEXTUREs, the pointer to the corresponding texture object
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T_Table}
				});

		//Disk
		M_Disk.init(this, "models/disk.obj");
		T_Disk.init(this, "textures/disk.png");
		DS_Disk.init(this, &DSLobj, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_Disk}
			});
		//Paddles
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
		//Ground
		M_Ground.init(this, "models/ground.obj");
		T_Ground.init(this, "textures/ground.png");
		DS_Ground.init(this, &DSLobj, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_Ground}
			});

		//Point Counters
		M_PointCounter.init(this, "models/point_counter.obj");
		T_PointCounter.init(this, "textures/point_counter.png");
		DS_PointCounter1.init(this, &DSLobj, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_PointCounter}
			});
		DS_PointCounter2.init(this, &DSLobj, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_PointCounter}
			});

		//Points
		M_Point.init(this, "models/point.obj");
		DS_PointRed1.init(this, &DSLobj, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_Paddle1}
			});

		DS_PointRed2.init(this, &DSLobj, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_Paddle1}
			});

		DS_PointRed3.init(this, &DSLobj, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_Paddle1}
			});
		DS_PointRed4.init(this, &DSLobj, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_Paddle1}
			});
		DS_PointRed5.init(this, &DSLobj, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_Paddle1}
			});
		DS_PointBlue1.init(this, &DSLobj, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_Paddle2}
			});
		DS_PointBlue2.init(this, &DSLobj, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_Paddle2}
			});
		DS_PointBlue3.init(this, &DSLobj, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_Paddle2}
			});
		DS_PointBlue4.init(this, &DSLobj, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_Paddle2}
			});
		DS_PointBlue5.init(this, &DSLobj, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_Paddle2}
			});
		//Starting interface
		M_Menu.init(this, "models/ground.obj");
		T_Menu.init(this, "textures/menu.png");
		DS_Menu.init(this, &DSLobj, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &T_Menu}
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
		gubo.switchLight = switchLight;
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


		 M_PointCounter.cleanup();
		 T_PointCounter.cleanup();
		 DS_PointCounter1.cleanup();
		 DS_PointCounter2.cleanup();


		M_Point.cleanup();
		DS_PointRed1.cleanup();
		DS_PointRed2.cleanup();
		DS_PointRed3.cleanup();
		DS_PointRed4.cleanup();
		DS_PointRed5.cleanup();
		DS_PointBlue1.cleanup();
		DS_PointBlue2.cleanup();
		DS_PointBlue3.cleanup();
		DS_PointBlue4.cleanup();
		DS_PointBlue5.cleanup();

		DS_Menu.cleanup();
		T_Menu.cleanup();
		M_Menu.cleanup();



		 P1.cleanup();
		 DS_global.cleanup();

		 DSLglobal.cleanup();
		 DSLobj.cleanup();
	}

	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				P1.graphicsPipeline);

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 0, 1, &DS_global.descriptorSets[currentImage],
			0, nullptr);

		//Table buffer initialization

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


		//PointCounter buffer initialization

		VkBuffer vertexBuffers_PointCounter[] = { M_PointCounter.vertexBuffer };
		VkDeviceSize offsets_PointCounter[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers_PointCounter, offsets_PointCounter);
		vkCmdBindIndexBuffer(commandBuffer, M_PointCounter.indexBuffer, 0,
			VK_INDEX_TYPE_UINT32);
		//PointCounter1

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &DS_PointCounter1.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_PointCounter.indices.size()), 1, 0, 0, 0);
		//PointCounter2

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &DS_PointCounter2.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_PointCounter.indices.size()), 1, 0, 0, 0);


		//Point buffer initialization

		VkBuffer vertexBuffers_Point[] = { M_Point.vertexBuffer };
		VkDeviceSize offsets_Point[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers_Point, offsets_Point);
		vkCmdBindIndexBuffer(commandBuffer, M_Point.indexBuffer, 0,
			VK_INDEX_TYPE_UINT32);
		//PointRed1

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &DS_PointRed1.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_Point.indices.size()), 1, 0, 0, 0);
		//PointRed2

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &DS_PointRed2.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_Point.indices.size()), 1, 0, 0, 0);
		//PointRed3

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &DS_PointRed3.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_Point.indices.size()), 1, 0, 0, 0);
		//PointRed4

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &DS_PointRed4.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_Point.indices.size()), 1, 0, 0, 0);
		//PointRed5

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &DS_PointRed5.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_Point.indices.size()), 1, 0, 0, 0);



		//PointBlue1

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &DS_PointBlue1.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_Point.indices.size()), 1, 0, 0, 0);
		//PointBlue2

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &DS_PointBlue2.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_Point.indices.size()), 1, 0, 0, 0);
		//PointBlue3

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &DS_PointBlue3.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_Point.indices.size()), 1, 0, 0, 0);
		//PointBlue4

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &DS_PointBlue4.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_Point.indices.size()), 1, 0, 0, 0);
		//PointBlue5

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &DS_PointBlue5.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_Point.indices.size()), 1, 0, 0, 0);


		//Starting Menu
		VkBuffer vertexBuffers_Menu[] = { M_Menu.vertexBuffer };
		VkDeviceSize offsets_Menu[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers_Menu, offsets_Menu);
		vkCmdBindIndexBuffer(commandBuffer, M_Menu.indexBuffer, 0,
			VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 1, 1, &DS_Menu.descriptorSets[currentImage],
			0, nullptr);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(M_Menu.indices.size()), 1, 0, 0, 0);
	}



	//Functions to manage points
	void goalRed() {
		if (scoreRed == 5) {
			scoreRed = 0;
			scoreBlue = 0;
		}
		else
		{
			scoreRed++;
		}
		paddle1.setPos(glm::vec3(-0.57f, 0.0f, 0.0f));
		paddle2.setPos(glm::vec3(0.57f, 0.0f, 0.0f));
	}

	void goalBlue() {
		if (scoreBlue == 5) {
			scoreRed = 0;
			scoreBlue = 0;
		}
		else {
			scoreBlue++;
		}

		paddle1.setPos(glm::vec3(-0.57f, 0.0f, 0.0f));
		paddle2.setPos(glm::vec3(0.57f, 0.0f, 0.0f));
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {

		//Time variables

		static auto startTime = std::chrono::high_resolution_clock::now();
		static float lastTime = 0.0f;

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>
			(currentTime - startTime).count();
		float deltaT = time - lastTime;
		lastTime = time;

		static float debounce = time;




	    //variables for input management

		float static yaw_p1 = 0.0f, pitch_p1 = 0.0f, roll_p1;
		float static yaw_p2 = 0.0f, pitch_p2 = 0.0f, roll_p2;
		float mx_p1 = 0, my_p1 = 0, mz_p1 = 0;
		float mx_p2 = 0, my_p2 = 0, mz_p2 = 0;
		float m_change;


	    //inputs capture

		//paddle1
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

		//paddle2
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

		//utility input
		if (glfwGetKey(window, GLFW_KEY_Q)) {
			paddle1.updatespeed(1,deltaT);
			paddle2.updatespeed(1, deltaT);
		}
		if (glfwGetKey(window, GLFW_KEY_E)) {
			paddle1.updatespeed(0, deltaT);
			paddle2.updatespeed(0, deltaT);
		}
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
			if (time - debounce > 0.5) {
				scoreBlue = 0;
				scoreRed = 0;
				paddle1.setPos(glm::vec3(-0.57f, 0.0f, 0.0f));
				paddle2.setPos(glm::vec3(0.57f, 0.0f, 0.0f));
				disk.setPos(glm::vec3(0.0f, 0.0f, 0.0f));
				disk.setSpeed(glm::vec2(0.0f, 0.0f));
				paddle1.setSpeed(1.15);
				paddle2.setSpeed(1.15);
				debounce = time;

			}
		}
		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
			if (time - debounce > 0.5) {
				if (switchLight.y == 0.0f) {
					switchLight = switchLight + glm::vec3(0.0f, 1.0f, 0.0f);
				}
				else
				{
					switchLight = switchLight - glm::vec3(0.0f, 1.0f, 0.0f);
				}


				debounce = time;

			}
		}
		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
			if (time - debounce > 0.5) {
				if (switchLight.x == 0.0f) {
					switchLight = switchLight + glm::vec3(1.0f, 0.0f, 0.0f);
				}
				else
				{
					switchLight = switchLight - glm::vec3(1.0f, 0.0f, 0.0f);
				}

				debounce = time;

			}
		}
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
			if (time - debounce > 0.5) {
				if (switchLight.z == 0.0f) {
					switchLight = switchLight + glm::vec3(0.0f, 0.0f, 1.0f);
				}
				else
				{
					switchLight = switchLight - glm::vec3(0.0f, 0.0f, 1.0f);
				}

				debounce = time;

			}
		}
		if (glfwGetKey(window, GLFW_KEY_O)) {

			if (radiusSpotLight >= 10.0f){
				radiusSpotLight -= 20.0 * deltaT;
				return;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_P)) {
			if (radiusSpotLight <= 40.0f)
				radiusSpotLight += 20.0 * deltaT;
			return;
		}

		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
			if (time - debounce > 0.5) {
				if (view == 3) {
					scoreBlue = 0;
					scoreRed = 0;
					paddle1.setPos(glm::vec3(-0.57f, 0.0f, 0.0f));
					paddle2.setPos(glm::vec3(0.57f, 0.0f, 0.0f));
					disk.setPos(glm::vec3(0.0f, 0.0f, 0.0f));
					disk.setSpeed(glm::vec2(0.0f, 0.0f));
					paddle1.setSpeed(1.15);
					paddle2.setSpeed(1.15);
					switchLight = glm::vec3(1.0f, 1.0f, 1.0f);
				}
				view=0;
				debounce = time;

			}
		}
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
			if (time - debounce > 0.5) {
				view=3;
				switchLight = glm::vec3(0.0f, 0.0f, 0.0f);
				debounce = time;

			}
		}

		UniformBufferObject ubo{};


		void* data;



	    //change of camera view

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			if (time - debounce > 0.5) {
				view++;
				debounce = time;
				view = view % 3;
			}
		}
		if (view == 0) {
			gubo.view = glm::lookAt(glm::vec3(0.0f, 1.0f, 1.0f),
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
		if (view == 3) {
			gubo.view = glm::lookAt(glm::vec3(0.0f, 10.0f, 0.000000001f),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
			gubo.eyePos = glm::vec3(0.0f, 1.0f, 1.0f);
		}



		//Disk update
		int status = disk.updateDisk(deltaT, glm::vec2(paddle1.getPos().x, paddle1.getPos().z), glm::vec2(paddle2.getPos().x, paddle2.getPos().z), glm::vec2(mx_p1, -mz_p1), glm::vec2(mx_p2, -mz_p2));

		switch (status) {
		case 0:
			break;
		case 1:
			goalBlue();
			break;
		case 2:
			goalRed();
			break;
		}

		//Paddles update
		paddle1.updatePaddle1(deltaT,mx_p1,mz_p1);
		paddle2.updatePaddle2(deltaT, mx_p2, mz_p2);


		//Adding part for illumination in shader
		gubo.lightColor = glm::vec3(1.0f, 1.0f, 1.0f)*0.2f;
		gubo.lightDir = glm::vec3(cos(glm::radians(90.0f)) * cos(glm::radians(0.0f)), sin(glm::radians(90.0f)), cos(glm::radians(90.0f)) * sin(glm::radians(0.0f)));
		gubo.lightPos = glm::vec3(0.0, 1.0, 0.0);
		gubo.lightParams = glm::vec4(
			cos(glm::radians(5.0f)), cos(glm::radians(radiusSpotLight)), 2.0f, 1.8f
		);
		gubo.switchLight = switchLight;
		gubo.gamma = 50.0f;



		vkMapMemory(device, DS_global.uniformBuffersMemory[0][currentImage], 0,
			sizeof(gubo), 0, &data);
		memcpy(data, &gubo, sizeof(gubo));
		vkUnmapMemory(device, DS_global.uniformBuffersMemory[0][currentImage]);


		//Table
		ubo.model = glm::mat4(1.0f);


		vkMapMemory(device, DS_Table.uniformBuffersMemory[0][currentImage], 0,
							sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_Table.uniformBuffersMemory[0][currentImage]);


		//Disk
		ubo.model = glm::translate(glm::mat4(1.0f),glm::vec3(disk.getPos().x, 0.0f, disk.getPos().y));


		vkMapMemory(device, DS_Disk.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_Disk.uniformBuffersMemory[0][currentImage]);

		//First paddle
		ubo.model = glm::translate(glm::mat4(1.0), glm::vec3(paddle1.getPos().x, paddle1.getPos().y, paddle1.getPos().z)) *
			glm::rotate(glm::mat4(1.0), glm::radians(roll_p1), glm::vec3(0, 1, 0)) *
			glm::rotate(glm::mat4(1.0), glm::radians(pitch_p1), glm::vec3(1, 0, 0)) *
			glm::rotate(glm::mat4(1.0), glm::radians(yaw_p1), glm::vec3(0, 0, 1));


		vkMapMemory(device, DS_Paddle1.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_Paddle1.uniformBuffersMemory[0][currentImage]);

		//Second paddle
		ubo.model = glm::translate(glm::mat4(1.0), glm::vec3(paddle2.getPos().x, paddle2.getPos().y, paddle2.getPos().z)) *
			glm::rotate(glm::mat4(1.0), glm::radians(roll_p2), glm::vec3(0, 1, 0)) *
			glm::rotate(glm::mat4(1.0), glm::radians(pitch_p2), glm::vec3(1, 0, 0)) *
			glm::rotate(glm::mat4(1.0), glm::radians(yaw_p2), glm::vec3(0, 0, 1));


		vkMapMemory(device, DS_Paddle2.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_Paddle2.uniformBuffersMemory[0][currentImage]);

		//Ground
		ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.7f, 0.0f));

		vkMapMemory(device, DS_Ground.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_Ground.uniformBuffersMemory[0][currentImage]);


		//Management of points translations
		float deltaTransPoint = 0.22;
		int moveRed1 = 0;
		int moveRed2 = 0;
		int moveRed3 = 0;
		int moveRed4 = 0;
		int moveRed5 = 0;

		switch (scoreRed) {
		case 1:
			moveRed1 = 1;
			break;
		case 2:
			moveRed1 = 1;
			moveRed2 = 1;
			break;
		case 3:
			moveRed1 = 1;
			moveRed2 = 1;
			moveRed3 = 1;
			break;
		case 4:
			moveRed1 = 1;
			moveRed2 = 1;
			moveRed3 = 1;
			moveRed4 = 1;
			break;
		case 5:
			moveRed1 = 1;
			moveRed2 = 1;
			moveRed3 = 1;
			moveRed4 = 1;
			moveRed5 = 1;
			break;
		}

		int moveBlue1 = 0;
		int moveBlue2 = 0;
		int moveBlue3 = 0;
		int moveBlue4 = 0;
		int moveBlue5 = 0;

		switch (scoreBlue) {
		case 1:
			moveBlue1 = 1;
			break;
		case 2:
			moveBlue1 = 1;
			moveBlue2 = 1;
			break;
		case 3:
			moveBlue1 = 1;
			moveBlue2 = 1;
			moveBlue3 = 1;
			break;
		case 4:
			moveBlue1 = 1;
			moveBlue2 = 1;
			moveBlue3 = 1;
			moveBlue4 = 1;
			break;
		case 5:
			moveBlue1 = 1;
			moveBlue2 = 1;
			moveBlue3 = 1;
			moveBlue4 = 1;
			moveBlue5 = 1;
			break;
		}


		//PointCounter1
		ubo.model = glm::mat4(1.0f) * glm::translate(glm::mat4(1.0f), glm::vec3(-0.9f, 0.0f, 0.0f));

		vkMapMemory(device, DS_PointCounter1.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_PointCounter1.uniformBuffersMemory[0][currentImage]);


		//PointCounter2
		ubo.model = glm::mat4(1.0f)* glm::translate(glm::mat4(1.0f), glm::vec3(0.9f, 0.0f, 0.0f));

		vkMapMemory(device, DS_PointCounter2.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_PointCounter2.uniformBuffersMemory[0][currentImage]);


		//PointRed1
		ubo.model = glm::mat4(1.0f) *glm::translate(glm::mat4(1.0f), glm::vec3(-0.9f, 0.0f, 0.05f-moveRed1*deltaTransPoint));
		vkMapMemory(device, DS_PointRed1.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_PointRed1.uniformBuffersMemory[0][currentImage]);

		//PointRed2
		ubo.model = glm::mat4(1.0f) * glm::translate(glm::mat4(1.0f), glm::vec3(-0.9f, 0.0f, 0.08f - moveRed2 * deltaTransPoint));
		vkMapMemory(device, DS_PointRed2.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_PointRed2.uniformBuffersMemory[0][currentImage]);

		//PointRed3
		ubo.model = glm::mat4(1.0f) * glm::translate(glm::mat4(1.0f), glm::vec3(-0.9f, 0.0f, 0.11f - moveRed3 * deltaTransPoint));
		vkMapMemory(device, DS_PointRed3.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_PointRed3.uniformBuffersMemory[0][currentImage]);

		//PointRed4
		ubo.model = glm::mat4(1.0f) * glm::translate(glm::mat4(1.0f), glm::vec3(-0.9f, 0.0f, 0.14f - moveRed4 * deltaTransPoint));
		vkMapMemory(device, DS_PointRed4.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_PointRed4.uniformBuffersMemory[0][currentImage]);

		//PointRed5
		ubo.model = glm::mat4(1.0f) * glm::translate(glm::mat4(1.0f), glm::vec3(-0.9f, 0.0f, 0.17f - moveRed5 * deltaTransPoint));
		vkMapMemory(device, DS_PointRed5.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_PointRed5.uniformBuffersMemory[0][currentImage]);


		//PointBlue1
		ubo.model = glm::mat4(1.0f) * glm::translate(glm::mat4(1.0f), glm::vec3(0.9f, 0.0f, -0.05f+moveBlue1 * deltaTransPoint));
		vkMapMemory(device, DS_PointBlue1.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_PointBlue1.uniformBuffersMemory[0][currentImage]);

		//PointBlue2
		ubo.model = glm::mat4(1.0f) * glm::translate(glm::mat4(1.0f), glm::vec3(0.9f, 0.0f, -0.08f + moveBlue2 * deltaTransPoint));
		vkMapMemory(device, DS_PointBlue2.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_PointBlue2.uniformBuffersMemory[0][currentImage]);

		//PointBlue3
		ubo.model = glm::mat4(1.0f) * glm::translate(glm::mat4(1.0f), glm::vec3(0.9f, 0.0f, -0.11f + moveBlue3 * deltaTransPoint));
		vkMapMemory(device, DS_PointBlue3.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_PointBlue3.uniformBuffersMemory[0][currentImage]);

		//PointBlue4
		ubo.model = glm::mat4(1.0f) * glm::translate(glm::mat4(1.0f), glm::vec3(0.9f, 0.0f, -0.14f + moveBlue4 * deltaTransPoint));
		vkMapMemory(device, DS_PointBlue4.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_PointBlue4.uniformBuffersMemory[0][currentImage]);

		//PointBlue5
		ubo.model = glm::mat4(1.0f) * glm::translate(glm::mat4(1.0f), glm::vec3(0.9f, 0.0f, -0.17f + moveBlue5 * deltaTransPoint));
		vkMapMemory(device, DS_PointBlue5.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_PointBlue5.uniformBuffersMemory[0][currentImage]);

		//Starting Menu
		ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f))*
			glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(0, 1, 0))*
			glm::scale(glm::mat4(1.0),glm::vec3(0.8f, 0.5f, 0.4f));

		vkMapMemory(device, DS_Menu.uniformBuffersMemory[0][currentImage], 0,
			sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_Menu.uniformBuffersMemory[0][currentImage]);

	}
};


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
