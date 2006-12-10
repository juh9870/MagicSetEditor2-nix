//+----------------------------------------------------------------------------+
//| Description:  Magic Set Editor - Program to make Magic (tm) cards          |
//| Copyright:    (C) 2001 - 2006 Twan van Laarhoven                           |
//| License:      GNU General Public License 2 or later (see file COPYING)     |
//+----------------------------------------------------------------------------+

#ifndef HEADER_DATA_SET
#define HEADER_DATA_SET

// ----------------------------------------------------------------------------- : Includes

#include <util/prec.hpp>
#include <util/reflect.hpp>
#include <util/action_stack.hpp>
#include <util/io/package.hpp>
#include <boost/scoped_ptr.hpp>

DECLARE_POINTER_TYPE(Card);
DECLARE_POINTER_TYPE(Set);
DECLARE_POINTER_TYPE(Game);
DECLARE_POINTER_TYPE(StyleSheet);
DECLARE_POINTER_TYPE(Styling);
DECLARE_POINTER_TYPE(Field);
DECLARE_POINTER_TYPE(Value);
DECLARE_POINTER_TYPE(Keyword);
DECLARE_INTRUSIVE_POINTER_TYPE(ScriptValue);
class ScriptManager;
class Context;
class Dependency;
template <typename> class OrderCache;
typedef shared_ptr<OrderCache<CardP> > OrderCacheP;

// ----------------------------------------------------------------------------- : Set

/// A set of cards
class Set : public Packaged {
  public:
	/// Create a set, the set should be open()ed later
	Set();
	/// Create a set using the given game
	Set(const GameP& game);
	/// Create a set using the given stylesheet, and its game
	Set(const StyleSheetP& stylesheet);
	~Set();
  
	GameP game;					///< The game this set uses
	StyleSheetP stylesheet;		///< The default stylesheet
	/// The values on the fields of the set
	/** The indices should correspond to the set_fields in the Game */
	IndexMap<FieldP, ValueP> data;
	/// Extra values for specitic stylesheets, indexed by stylesheet name
	DECLARE_POINTER_TYPE(Styling);
	map<String, StylingP> styling_data;
	vector<CardP> cards;		///< The cards in the set
	vector<KeywordP> keywords;	///< Additional keywords used in this set
	String apprentice_code;		///< Code to use for apprentice (Magic only)
	ActionStack actions;		///< Actions performed on this set and the cards in it
	
	/// A context for performing scripts
	/** Should only be used from the main thread! */
	Context& getContext();
	/// A context for performing scripts on a particular card
	/** Should only be used from the main thread! */
	Context& getContext(const CardP& card);
	/// Update styles for a card
	void updateFor(const CardP& card);
	
	/// Stylesheet to use for a particular card
	/** card may be null */
	StyleSheetP stylesheetFor(const CardP& card);
	
	/// Styling information for a particular stylesheet
	IndexMap<FieldP, ValueP>& stylingDataFor(const StyleSheet&);
	
	/// Find a value in the data by name and type
	template <typename T> T& value(const String& name) {
		for(IndexMap<FieldP, ValueP>::iterator it = data.begin() ; it != data.end() ; ++it) {
			if ((*it)->fieldP->name == name) {
				T* ret = dynamic_cast<T*>(it->get());
				if (!ret) throw InternalError(_("Set field with name '")+name+_("' doesn't have the right type"));
				return *ret;
			}
		}
		throw InternalError(_("Expected a set field with name '")+name+_("'"));
	}
	
	/// Find the position of a card in this set, when the card list is sorted using the given cirterium
	int positionOfCard(const CardP& card, const ScriptValueP& order_by);
	
  protected:
	virtual String typeName() const;
	virtual void validate(Version);
	
	DECLARE_REFLECTION();
  private:
	/// Object for managing and executing scripts
	scoped_ptr<ScriptManager> script_manager;
	/// Cache of cards ordered by some criterion
	map<ScriptValueP,OrderCacheP> order_cache;
};

inline int item_count(const Set& set) {
	return (int)set.cards.size();
}
ScriptValueP make_iterator(const Set& set);

void mark_dependency_member(const SetP& value, const String& name, const Dependency& dep);
void mark_dependency_member(Set*        value, const String& name, const Dependency& dep);

// ----------------------------------------------------------------------------- : SetView

/// A 'view' of a Set, is notified when the Set is updated
/** To listen to events, derived classes should override onAction(const Action&, bool undone)
 */
class SetView : public ActionListener {
  public:
	SetView();
	~SetView();
  
	/// Get the set that is currently being viewed
	inline SetP getSet() { return set; }
	/// Change the set that is being viewed
	void setSet(const SetP& set);
	
  protected:
	/// The set that is currently being viewed, should not be modified directly!
	SetP set;
	
	/// Called when another set is being viewed (using setSet)
	virtual void onChangeSet() {}
	/// Called when just before another set is being viewed (using setSet)
	virtual void onBeforeChangeSet() {}
};


// ----------------------------------------------------------------------------- : EOF
#endif
