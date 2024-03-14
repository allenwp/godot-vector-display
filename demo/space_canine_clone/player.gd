extends Node

@export var max_pitch_yaw: Vector2 = Vector2(0.4, 0.4);
@export var max_rotate_speed: float = 0.1
@export var settle_pitch_yaw_tween_steps: float = 4
@export var translate_speed: float = 1.0
@export var camera_rotation_scale: float = 0.5
## This is based on FOV of the camera and distance of the ship to the camera.
## Hardcoded rather than calculated based on FOV because this allows FOV to
## change with visual effects and it's a "tweek until it looks right" thing anyway.
@export var camera_bounds_reduction: Vector2 = Vector2(30, 30)

@export var sawtooth_cooldown_time: float = 0.25

@export var time_pause_time: float = 2
@export var time_pause_cooldown_time: float = 5

var ship_pitch_yaw: Vector2 = Vector2.ZERO

var sawtooth_cooldown: float = 0;
var needs_cache_shot: bool = true
var laser_scene := preload("res://space_canine_clone/laser.tscn")

@onready var dolly: Dolly = %Dolly
@onready var player_ship: Node3D = %PlayerShip
@onready var player_camera: Node3D = %PlayerCamera
@onready var laser_spawn_point: Node3D = %LaserSpawnPoint

func _process(delta: float) -> void:
#	if needs_cache_shot:
#		needs_cache_shot = false
#		var laser: Laser = laser_scene.instantiate()
#		add_child(laser)
#		laser.global_transform = laser_spawn_point.global_transform
#		laser.global_position += Vector3(0,-10,-15)
#		laser.initial_position = laser.global_position
#
#		var laser2: Laser = laser_scene.instantiate()
#		add_child(laser2)
#		laser2.global_transform = laser2.global_transform
#		laser2.initial_position = laser2.global_position


	sawtooth_cooldown -= delta
	sawtooth_cooldown = maxf(sawtooth_cooldown, 0)
	if (sawtooth_cooldown == 0):
		if Input.is_action_pressed("shoot_sawtooth"):
			sawtooth_cooldown = sawtooth_cooldown_time
			var laser: Laser = laser_scene.instantiate()
			add_child(laser)
			laser.global_transform = laser_spawn_point.global_transform
			laser.initial_position = laser.global_position

func _physics_process(_delta: float) -> void:
	var input: Vector2 = Vector2.ZERO

	input = Input.get_vector("move_down", "move_up", "move_right", "move_left")

	var target_pitch_yaw := max_pitch_yaw * input

	if (player_ship.position.y < dolly.ground + 0.1):
		target_pitch_yaw.x = maxf (0.0, target_pitch_yaw.x)

	ship_pitch_yaw = speed_limited_tween(ship_pitch_yaw, target_pitch_yaw, settle_pitch_yaw_tween_steps, max_rotate_speed)

	player_ship.position.x += ship_pitch_yaw.y * -1.0 * translate_speed
	player_ship.position.y += ship_pitch_yaw.x * translate_speed
	player_ship.position.x = clampf(player_ship.position.x, dolly.bounds.x / -2.0, dolly.bounds.x / 2.0)
	player_ship.position.y = clampf(player_ship.position.y, maxf(dolly.bounds.y / -2.0, dolly.ground), dolly.bounds.y / 2.0)

	player_ship.quaternion = Quaternion.from_euler(Vector3(ship_pitch_yaw.x, ship_pitch_yaw.y, ship_pitch_yaw.y))

	var camera_pitch_yaw := ship_pitch_yaw * camera_rotation_scale
	player_camera.quaternion = Quaternion.from_euler(Vector3(camera_pitch_yaw.x, camera_pitch_yaw.y, camera_pitch_yaw.y))

	var camera_bounds: Vector2 = dolly.bounds - camera_bounds_reduction
	var ship_position_ratio: Vector2 = Vector2(player_ship.position.x / (dolly.bounds.x / 2.0), player_ship.position.y / (dolly.bounds.y / 2.0))
	player_camera.position.x = ship_position_ratio.x * (camera_bounds.x / 2.0)
	player_camera.position.y = ship_position_ratio.y * (camera_bounds.y / 2.0)


## A smaller num_steps will make the tween happen faster.
## max_speed will limit the speed of the tween.
func speed_limited_tween(start: Vector2, end: Vector2, num_steps: float, max_speed: float) -> Vector2:
	var delta := (end - start) / num_steps
	delta = Vector2(
			clampf(delta.x, -1 * max_speed, max_speed),
			clampf(delta.y, -1 * max_speed, max_speed)
	)
	return start + delta

# Just an old function I'm not using anymore
#func clamp_abs_vector2(vector: Vector2, max_x: float, max_y: float) -> Vector2:
#	return Vector2(
#			clampf(vector.x, -1.0 * max_x, max_x),
#			clampf(vector.y, -1.0 * max_y, max_y),
#	)
