#pragma once

auto drawmenu() -> void
{
	if (GetAsyncKeyState(VK_INSERT) & 1) { Settings::bMenu = !Settings::bMenu; }

	if (Settings::bMenu)
	{
		ImGui::Begin("Valorant", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		ImGui::SetWindowSize("Valorant", ImVec2(300, 450));

		ImGui::Spacing();

		ImGui::Checkbox("Enable 2D Box", &Settings::Visuals::bBox);
		if (Settings::Visuals::bBox) Settings::Visuals::bBoxOutlined = false;

		ImGui::Checkbox("Enable Cornered Box", &Settings::Visuals::bBoxOutlined);
		if (Settings::Visuals::bBoxOutlined) Settings::Visuals::bBox = false;

		ImGui::Checkbox("Enable Snaplines", &Settings::Visuals::bSnaplines);
		ImGui::Checkbox("Enable Distance", &Settings::Visuals::bDistance);
		ImGui::Checkbox("Enable HealthBar", &Settings::Visuals::bHealth);

		ImGui::End();
	}
}