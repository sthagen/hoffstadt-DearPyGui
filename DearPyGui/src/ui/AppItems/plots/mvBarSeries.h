#pragma once

#include "mvPlot.h"

class mvBarSeries : public mvAppItem
{

public:

    explicit mvBarSeries(mvUUID uuid) : mvAppItem(uuid) {}

    void draw(ImDrawList* drawlist, float x, float y) override;
    void handleSpecificRequiredArgs(PyObject* args) override;
    void handleSpecificKeywordArgs(PyObject* dict) override;
    void getSpecificConfiguration(PyObject* dict) override;
    void applySpecificTemplate(mvAppItem* item) override;

    // values
    void setDataSource(mvUUID dataSource) override;
    void* getValue() override { return &_value; }
    PyObject* getPyValue() override;
    void setPyValue(PyObject* value) override;

private:

    bool _horizontal = false;
    float _weight = 1.0f;
    mvRef<std::vector<std::vector<double>>> _value = CreateRef<std::vector<std::vector<double>>>(
        std::vector<std::vector<double>>{ std::vector<double>{},
        std::vector<double>{},
        std::vector<double>{},
        std::vector<double>{},
        std::vector<double>{} });

};