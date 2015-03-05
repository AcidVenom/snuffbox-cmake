Game.targets = Game.targets || {
	default: new RenderTarget("Default")
}

Game.Initialise = function()
{
	RenderSettings.setResolution(1920, 1080);
	ContentManager.load("texture", "wood.png");
	ContentManager.load("texture", "wood_normal.png");

	ContentManager.load("texture", "metal.png");
	ContentManager.load("texture", "metal_normal.png");

	ContentManager.load("texture", "cube_map_left.png");
	ContentManager.load("texture", "cube_map_right.png");
	ContentManager.load("texture", "cube_map_top.png");
	ContentManager.load("texture", "cube_map_bottom.png");
	ContentManager.load("texture", "cube_map_front.png");
	ContentManager.load("texture", "cube_map_back.png");

	ContentManager.load("shader", "shaders/ui.fx");
	ContentManager.load("shader", "shaders/text.fx");
	ContentManager.load("shader", "shaders/skybox.fx");

	ContentManager.load("effect", "test.effect");
	ContentManager.load("effect", "post_processing.effect");
	ContentManager.load("material", "test.material");

	ContentManager.load("model", "axew.fbx");
	ContentManager.load("model", "skybox.fbx");

	Game.targets.default.setPostProcessing("post_processing.effect");

	Game.camera = new Camera(CameraType.Perspective);

	Game.light = new Light(LightType.Directional);

	Game.camera.setTranslation(-2.5, -22, 16);
	Game.camera.setRotation(0.66, -2.2, 0);
	
	Game.model = new Model("axew.fbx");
	Game.model.spawn("Default");
	Game.model.setMaterial("test.material");

	Game.skybox = new Model("skybox.fbx");
	Game.skybox.spawn("Default");
	Game.skybox.setTechnique("Skybox");
	Game.skybox.setMaterial("test.material");

	Game.text = new Text();
	Game.text.setText("Snuffbox - Work In Progress");
	Game.text.setFontSize(32);
	Game.text.setAlignment(TextAlignment.Right);
	Game.text.setTranslation(1920 / 2 - 20, 1080 / 2 - 20 - Game.text.metrics().h);
	Game.text.spawn("Default");
	Game.text.setShadowOffset(1, 1);
	Game.text.setShadowColour(0, 0, 0, 0.1);
	Game.text.setAlpha(0.25);

	Game.model.setScale(10, 10, 10);
	Game.skybox.setScale(5, 5, 5);

	Game.camera.setFarPlane(99999);
}

Game.Update = function(dt)
{
	if (Keyboard.isReleased(Key.Q))
	{
		Game.quit();
	}

	var mz = 0,
		mx = 0,
		rx = 0,
		ry = 0,
		speed = 20;

	if (Keyboard.isDown(Key.W))
	{
		mz = -dt * speed;
	}
	else if (Keyboard.isDown(Key.S))
	{
		mz = dt * speed;
	}

	if (Keyboard.isDown(Key.A))
	{
		mx = -dt * speed;
	}
	else if (Keyboard.isDown(Key.D))
	{
		mx = dt * speed;
	}

	if (Mouse.isDown(MouseButton.Left))
	{
		var movement = Mouse.movement();
		rx = movement.y / 100;
		ry = -movement.x / 100;
	}

	Game.camera.translateBy(mx, 0, mz);
	Game.camera.rotateBy(rx, ry, 0);

	Game.model.rotateBy(0, dt, 0);

	var t = Game.camera.translation();
	Game.skybox.setTranslation(t.x, -t.y, t.z);
}

Game.FixedUpdate = function(timeSteps, fixedDelta)
{
	
}

Game.Draw = function(dt)
{
	Game.render(Game.camera);
}

Game.Shutdown = function()
{

}

Game.OnReload = function(path)
{
	
}
