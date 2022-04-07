#include "mvPlotting.h"

#include <utility>
#include "mvCore.h"
#include "mvContext.h"
#include "mvItemRegistry.h"
#include "mvPythonExceptions.h"
#include "AppItems/mvFontItems.h"
#include "AppItems/mvThemes.h"
#include "AppItems/containers/mvDragPayload.h"
#include "mvPyObject.h"
#include "AppItems/plots/mvPlot.h"
#include "AppItems/plots/mvSubPlots.h"

void
DearPyGui::draw_plot_legend(ImDrawList* drawlist, mvAppItem& item, mvPlotLegendConfig& config)
{
	if (!item.config.show)
		return;

	if (config.dirty)
	{
		ImPlot::SetLegendLocation(config.legendLocation, config.horizontal ? ImPlotOrientation_Horizontal : ImPlotOrientation_Vertical, config.outside);
		config.dirty = false;
	}

	UpdateAppItemState(item.state);

	if (item.font)
	{
		ImGui::PopFont();
	}

	if (item.theme)
	{
		static_cast<mvTheme*>(item.theme.get())->customAction();
	}

	if (item.config.dropCallback)
	{
		if (ImPlot::BeginDragDropTargetLegend())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(item.config.payloadType.c_str()))
			{
				auto payloadActual = static_cast<const mvDragPayload*>(payload->Data);
				mvAddCallback(item.config.dropCallback, item.uuid, payloadActual->getDragData(), nullptr);
			}

			ImPlot::EndDragDropTarget();
		}
	}
}

void 
DearPyGui::set_data_source(mvAppItem& item, mvUUID dataSource, mvBarSeriesConfig& outConfig)
{
	if (dataSource == item.config.source) return;
	item.config.source = dataSource;

	mvAppItem* srcItem = GetItem((*GContext->itemRegistry), dataSource);
	if (!srcItem)
	{
		mvThrowPythonError(mvErrorCode::mvSourceNotFound, "set_value",
			"Source item not found: " + std::to_string(dataSource), &item);
		return;
	}
	if (DearPyGui::GetEntityValueType(srcItem->type) != DearPyGui::GetEntityValueType(item.type))
	{
		mvThrowPythonError(mvErrorCode::mvSourceNotCompatible, "set_value",
			"Values types do not match: " + std::to_string(dataSource), &item);
		return;
	}
	outConfig.value = *static_cast<std::shared_ptr<std::vector<std::vector<double>>>*>(srcItem->getValue());
}

void
DearPyGui::set_data_source(mvAppItem& item, mvUUID dataSource, mvBasicSeriesConfig& outConfig)
{
	if (dataSource == item.config.source) return;
	item.config.source = dataSource;

	mvAppItem* srcItem = GetItem((*GContext->itemRegistry), dataSource);
	if (!srcItem)
	{
		mvThrowPythonError(mvErrorCode::mvSourceNotFound, "set_value",
			"Source item not found: " + std::to_string(dataSource), &item);
		return;
	}
	if (DearPyGui::GetEntityValueType(srcItem->type) != DearPyGui::GetEntityValueType(item.type))
	{
		mvThrowPythonError(mvErrorCode::mvSourceNotCompatible, "set_value",
			"Values types do not match: " + std::to_string(dataSource), &item);
		return;
	}
	outConfig.value = *static_cast<std::shared_ptr<std::vector<std::vector<double>>>*>(srcItem->getValue());
}

void 
DearPyGui::draw_bar_series(ImDrawList* drawlist, mvAppItem& item, const mvBarSeriesConfig& config)
{

	//-----------------------------------------------------------------------------
	// pre draw
	//-----------------------------------------------------------------------------
	if (!item.config.show)
		return;

	// push font if a font object is attached
	if (item.font)
	{
		ImFont* fontptr = static_cast<mvFont*>(item.font.get())->getFontPtr();
		ImGui::PushFont(fontptr);
	}

	// themes
	apply_local_theming(&item);

	//-----------------------------------------------------------------------------
	// draw
	//-----------------------------------------------------------------------------
	{

		static const std::vector<double>* xptr;
		static const std::vector<double>* yptr;

		xptr = &(*config.value.get())[0];
		yptr = &(*config.value.get())[1];

		if (config.horizontal)
			ImPlot::PlotBarsH(item.info.internalLabel.c_str(), xptr->data(), yptr->data(), (int)xptr->size(), config.weight);
		else
			ImPlot::PlotBars(item.info.internalLabel.c_str(), xptr->data(), yptr->data(), (int)xptr->size(), config.weight);

		// Begin a popup for a legend entry.
		if (ImPlot::BeginLegendPopup(item.info.internalLabel.c_str(), 1))
		{
			for (auto& childset : item.childslots)
			{
				for (auto& item : childset)
				{
					// skip item if it's not shown
					if (!item->config.show)
						continue;
					item->draw(drawlist, ImPlot::GetPlotPos().x, ImPlot::GetPlotPos().y);
					UpdateAppItemState(item->state);
				}
			}
			ImPlot::EndLegendPopup();
		}
	}

	//-----------------------------------------------------------------------------
	// update state
	//   * only update if applicable
	//-----------------------------------------------------------------------------


	//-----------------------------------------------------------------------------
	// post draw
	//-----------------------------------------------------------------------------

	// pop font off stack
	if (item.font)
		ImGui::PopFont();

	// handle popping themes
	cleanup_local_theming(&item);
}

void
DearPyGui::draw_line_series(ImDrawList* drawlist, mvAppItem& item, const mvBasicSeriesConfig& config)
{
	//-----------------------------------------------------------------------------
	// pre draw
	//-----------------------------------------------------------------------------
	if (!item.config.show)
		return;

	// push font if a font object is attached
	if (item.font)
	{
		ImFont* fontptr = static_cast<mvFont*>(item.font.get())->getFontPtr();
		ImGui::PushFont(fontptr);
	}

	// themes
	apply_local_theming(&item);

	//-----------------------------------------------------------------------------
	// draw
	//-----------------------------------------------------------------------------
	{

		static const std::vector<double>* xptr;
		static const std::vector<double>* yptr;

		xptr = &(*config.value.get())[0];
		yptr = &(*config.value.get())[1];

		ImPlot::PlotLine(item.info.internalLabel.c_str(), xptr->data(), yptr->data(), (int)xptr->size());

		// Begin a popup for a legend entry.
		if (ImPlot::BeginLegendPopup(item.info.internalLabel.c_str(), 1))
		{
			for (auto& childset : item.childslots)
			{
				for (auto& item : childset)
				{
					// skip item if it's not shown
					if (!item->config.show)
						continue;
					item->draw(drawlist, ImPlot::GetPlotPos().x, ImPlot::GetPlotPos().y);
					UpdateAppItemState(item->state);
				}
			}
			ImPlot::EndLegendPopup();
		}
	}

	//-----------------------------------------------------------------------------
	// update state
	//   * only update if applicable
	//-----------------------------------------------------------------------------


	//-----------------------------------------------------------------------------
	// post draw
	//-----------------------------------------------------------------------------

	// pop font off stack
	if (item.font)
		ImGui::PopFont();

	// handle popping themes
	cleanup_local_theming(&item);
}

void
DearPyGui::draw_scatter_series(ImDrawList* drawlist, mvAppItem& item, const mvBasicSeriesConfig& config)
{
	//-----------------------------------------------------------------------------
	// pre draw
	//-----------------------------------------------------------------------------
	if (!item.config.show)
		return;

	// push font if a font object is attached
	if (item.font)
	{
		ImFont* fontptr = static_cast<mvFont*>(item.font.get())->getFontPtr();
		ImGui::PushFont(fontptr);
	}

	// themes
	apply_local_theming(&item);

	//-----------------------------------------------------------------------------
	// draw
	//-----------------------------------------------------------------------------
	{

		static const std::vector<double>* xptr;
		static const std::vector<double>* yptr;

		xptr = &(*config.value.get())[0];
		yptr = &(*config.value.get())[1];

		ImPlot::PlotScatter(item.info.internalLabel.c_str(), xptr->data(), yptr->data(), (int)xptr->size());

		// Begin a popup for a legend entry.
		if (ImPlot::BeginLegendPopup(item.info.internalLabel.c_str(), 1))
		{
			for (auto& childset : item.childslots)
			{
				for (auto& item : childset)
				{
					// skip item if it's not shown
					if (!item->config.show)
						continue;
					item->draw(drawlist, ImPlot::GetPlotPos().x, ImPlot::GetPlotPos().y);
					UpdateAppItemState(item->state);
				}
			}
			ImPlot::EndLegendPopup();
		}
	}

	//-----------------------------------------------------------------------------
	// update state
	//   * only update if applicable
	//-----------------------------------------------------------------------------


	//-----------------------------------------------------------------------------
	// post draw
	//-----------------------------------------------------------------------------
	// 
	// pop font off stack
	if (item.font)
		ImGui::PopFont();

	// handle popping themes
	cleanup_local_theming(&item);
}

void
DearPyGui::draw_stair_series(ImDrawList* drawlist, mvAppItem& item, const mvBasicSeriesConfig& config)
{
	//-----------------------------------------------------------------------------
	// pre draw
	//-----------------------------------------------------------------------------
	if (!item.config.show)
		return;

	// push font if a font object is attached
	if (item.font)
	{
		ImFont* fontptr = static_cast<mvFont*>(item.font.get())->getFontPtr();
		ImGui::PushFont(fontptr);
	}

	// themes
	apply_local_theming(&item);

	//-----------------------------------------------------------------------------
	// draw
	//-----------------------------------------------------------------------------
	{

		static const std::vector<double>* xptr;
		static const std::vector<double>* yptr;

		xptr = &(*config.value.get())[0];
		yptr = &(*config.value.get())[1];

		ImPlot::PlotStairs(item.info.internalLabel.c_str(), xptr->data(), yptr->data(), (int)xptr->size());

		// Begin a popup for a legend entry.
		if (ImPlot::BeginLegendPopup(item.info.internalLabel.c_str(), 1))
		{
			for (auto& childset : item.childslots)
			{
				for (auto& item : childset)
				{
					// skip item if it's not shown
					if (!item->config.show)
						continue;
					item->draw(drawlist, ImPlot::GetPlotPos().x, ImPlot::GetPlotPos().y);
					UpdateAppItemState(item->state);
				}
			}
			ImPlot::EndLegendPopup();
		}
	}

	//-----------------------------------------------------------------------------
	// update state
	//   * only update if applicable
	//-----------------------------------------------------------------------------


	//-----------------------------------------------------------------------------
	// post draw
	//-----------------------------------------------------------------------------
	// 
	// pop font off stack
	if (item.font)
		ImGui::PopFont();

	// handle popping themes
	cleanup_local_theming(&item);
}

void
DearPyGui::draw_stem_series(ImDrawList* drawlist, mvAppItem& item, const mvBasicSeriesConfig& config)
{
	//-----------------------------------------------------------------------------
	// pre draw
	//-----------------------------------------------------------------------------
	if (!item.config.show)
		return;

	// push font if a font object is attached
	if (item.font)
	{
		ImFont* fontptr = static_cast<mvFont*>(item.font.get())->getFontPtr();
		ImGui::PushFont(fontptr);
	}

	// themes
	apply_local_theming(&item);

	//-----------------------------------------------------------------------------
	// draw
	//-----------------------------------------------------------------------------
	{

		static const std::vector<double>* xptr;
		static const std::vector<double>* yptr;

		xptr = &(*config.value.get())[0];
		yptr = &(*config.value.get())[1];

		ImPlot::PlotStems(item.info.internalLabel.c_str(), xptr->data(), yptr->data(), (int)xptr->size());

		// Begin a popup for a legend entry.
		if (ImPlot::BeginLegendPopup(item.info.internalLabel.c_str(), 1))
		{
			for (auto& childset : item.childslots)
			{
				for (auto& item : childset)
				{
					// skip item if it's not shown
					if (!item->config.show)
						continue;
					item->draw(drawlist, ImPlot::GetPlotPos().x, ImPlot::GetPlotPos().y);
					UpdateAppItemState(item->state);
				}
			}
			ImPlot::EndLegendPopup();
		}
	}

	//-----------------------------------------------------------------------------
	// update state
	//   * only update if applicable
	//-----------------------------------------------------------------------------


	//-----------------------------------------------------------------------------
	// post draw
	//-----------------------------------------------------------------------------
	// 
	// pop font off stack
	if (item.font)
		ImGui::PopFont();

	// handle popping themes
	cleanup_local_theming(&item);
}

void 
DearPyGui::set_required_configuration(PyObject* inDict, mvBarSeriesConfig& outConfig)
{
	if (!VerifyRequiredArguments(GetParsers()[GetEntityCommand(mvAppItemType::mvBarSeries)], inDict))
		return;

	(*outConfig.value)[0] = ToDoubleVect(PyTuple_GetItem(inDict, 0));
	(*outConfig.value)[1] = ToDoubleVect(PyTuple_GetItem(inDict, 1));
}

void
DearPyGui::set_required_configuration(PyObject* inDict, mvBasicSeriesConfig& outConfig)
{
	if (!VerifyRequiredArguments(GetParsers()[GetEntityCommand(mvAppItemType::mvLineSeries)], inDict))
		return;

	(*outConfig.value)[0] = ToDoubleVect(PyTuple_GetItem(inDict, 0));
	(*outConfig.value)[1] = ToDoubleVect(PyTuple_GetItem(inDict, 1));
}

void
DearPyGui::set_configuration(PyObject* inDict, mvPlotLegendConfig& outConfig, mvAppItem& item)
{
	if (inDict == nullptr)
		return;

	if (PyObject* item = PyDict_GetItemString(inDict, "location")) { outConfig.legendLocation = ToInt(item); outConfig.dirty = true; }
	if (PyObject* item = PyDict_GetItemString(inDict, "horizontal")) { outConfig.horizontal = ToBool(item); outConfig.dirty = true; }
	if (PyObject* item = PyDict_GetItemString(inDict, "outside")) { outConfig.outside = ToBool(item); outConfig.dirty = true; }

	if (item.info.shownLastFrame)
	{
		item.info.shownLastFrame = false;
		if (auto plot = static_cast<mvPlot*>(item.info.parentPtr))
			plot->removeFlag(ImPlotFlags_NoLegend);
		else if (auto plot = static_cast<mvSubPlots*>(item.info.parentPtr))
			plot->removeFlag(ImPlotSubplotFlags_NoLegend);
		item.config.show = true;
	}

	if (item.info.hiddenLastFrame)
	{
		item.info.hiddenLastFrame = false;
		if (auto plot = static_cast<mvPlot*>(item.info.parentPtr))
			plot->addFlag(ImPlotFlags_NoLegend);
		else if (auto plot = static_cast<mvSubPlots*>(item.info.parentPtr))
			plot->addFlag(ImPlotSubplotFlags_NoLegend);
		item.config.show = false;
	}
}

void
DearPyGui::set_configuration(PyObject* inDict, mvBarSeriesConfig& outConfig)
{
	if (inDict == nullptr)
		return;

	if (PyObject* item = PyDict_GetItemString(inDict, "horizontal")) outConfig.horizontal = ToBool(item);
	if (PyObject* item = PyDict_GetItemString(inDict, "weight")) outConfig.weight = ToFloat(item);
	if (PyObject* item = PyDict_GetItemString(inDict, "x")) { (*outConfig.value)[0] = ToDoubleVect(item); }
	if (PyObject* item = PyDict_GetItemString(inDict, "y")) { (*outConfig.value)[1] = ToDoubleVect(item); }

}

void 
DearPyGui::set_configuration(PyObject* inDict, mvBasicSeriesConfig& outConfig)
{
	if (inDict == nullptr)
		return;

	if (PyObject* item = PyDict_GetItemString(inDict, "x")) { (*outConfig.value)[0] = ToDoubleVect(item); }
	if (PyObject* item = PyDict_GetItemString(inDict, "y")) { (*outConfig.value)[1] = ToDoubleVect(item); }

}

void
DearPyGui::fill_configuration_dict(const mvPlotLegendConfig& inConfig, PyObject* outDict)
{
	if (outDict == nullptr)
		return;

	PyDict_SetItemString(outDict, "location", mvPyObject(ToPyInt(inConfig.legendLocation)));
	PyDict_SetItemString(outDict, "horizontal", mvPyObject(ToPyBool(inConfig.horizontal)));
	PyDict_SetItemString(outDict, "outside", mvPyObject(ToPyBool(inConfig.outside)));

}

void 
DearPyGui::fill_configuration_dict(const mvBarSeriesConfig& inConfig, PyObject* outDict)
{
	if (outDict == nullptr)
		return;

	mvPyObject py_horizontal = ToPyBool(inConfig.horizontal);
	mvPyObject py_weight = ToPyFloat(inConfig.weight);

	PyDict_SetItemString(outDict, "horizontal", py_horizontal);
	PyDict_SetItemString(outDict, "weight", py_weight);
}

void
DearPyGui::fill_configuration_dict(const mvBasicSeriesConfig& inConfig, PyObject* outDict)
{
	if (outDict == nullptr)
		return;
}

void
DearPyGui::apply_template(const mvPlotLegendConfig& sourceConfig, mvPlotLegendConfig& dstConfig)
{
	dstConfig.legendLocation = sourceConfig.legendLocation;
	dstConfig.horizontal = sourceConfig.horizontal;
	dstConfig.outside = sourceConfig.outside;
}

void 
DearPyGui::apply_template(const mvBarSeriesConfig& sourceConfig, mvBarSeriesConfig& dstConfig)
{
	dstConfig.value = sourceConfig.value;
	dstConfig.horizontal = sourceConfig.horizontal;
	dstConfig.weight = sourceConfig.weight;
}

void
DearPyGui::apply_template(const mvBasicSeriesConfig& sourceConfig, mvBasicSeriesConfig& dstConfig)
{
	dstConfig.value = sourceConfig.value;
}