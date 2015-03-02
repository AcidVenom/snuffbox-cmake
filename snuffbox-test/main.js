Game.targets = Game.targets || {
	default: new RenderTarget("Default")
}

Game.Initialise = function()
{
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

	ContentManager.load("effect", "test.effect");
	ContentManager.load("effect", "post_processing.effect");
	ContentManager.load("material", "test.material");

	Game.targets.default.setPostProcessing("post_processing.effect");

	Game.camera = new Camera(CameraType.Perspective);

	Game.terrain = new Terrain();
	Game.terrain.spawn("Default");
	Game.terrain.setMaterial("test.material");

	Game.light = new Light(LightType.Point);
	Game.light.setConstantAttenuation(0.01);
	Game.light.setLinearAttenuation(0.01);
	Game.light.setQuadraticAttenuation(0.01);

	Game.light.setTranslation(20, 2, 20);

	Game.light2 = new Light(LightType.Point);
	Game.light2.setConstantAttenuation(0.01);
	Game.light2.setLinearAttenuation(0.01);
	Game.light2.setQuadraticAttenuation(0.01);

	Game.light2.setTranslation(40, 2, 40);
	Game.light2.setColour(1, 0, 0, 1);

	Game.camera.setTranslation(-2.5, -22, 16);
	Game.camera.setRotation(0.66, -2.2, 0);
	
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
		speed = 5;

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

	var t = Game.time();
	Game.light.setTranslation(20 + Math.sin(t) * 10, 10, 20 + Math.cos(t) * 10);
	Game.light.setColour(0,1,1,1);
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
