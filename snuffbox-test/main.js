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

	ContentManager.load("shader", "shaders/ui.fx");
	ContentManager.load("effect", "test.effect");
	ContentManager.load("effect", "post_processing.effect");
	ContentManager.load("material", "test.material");

	ContentManager.load("model", "axew.fbx");

	Game.targets.default.setPostProcessing("post_processing.effect");

	Game.camera = new Camera(CameraType.Perspective);

	Game.terrain = new Terrain();
	Game.terrain.spawn("Default");
	Game.terrain.setMaterial("test.material");

	Game.light = new Light(LightType.Directional);

	Game.camera.setTranslation(-2.5, -22, 16);
	Game.camera.setRotation(0.66, -2.2, 0);
	
	Game.model = new Model("axew.fbx");
	Game.model.spawn("Default");
	Game.model.setMaterial("test.material");
	
	Game.model.setScale(10, 10, 10);

	Game.widget = new Widget();
	Game.widget.spawn("Default");
	Game.widget.setMaterial("test.material");
	Game.widget.setTechnique("UI");

	
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
