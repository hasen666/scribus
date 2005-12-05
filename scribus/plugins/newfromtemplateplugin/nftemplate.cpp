/***************************************************************************
 *   Riku Leino, tsoots@gmail.com                                          *
 ***************************************************************************/
#include <qstring.h>
#include <qcursor.h>
#include <qdir.h>
#include <qwidget.h>

#include "scribus.h"
#include "nftemplate.h"
#include "nftemplate.moc"
#include "nftdialog.h"
#include "scraction.h"
#include "menumanager.h"
#include "undomanager.h"
#include "prefsmanager.h"

int newfromtemplateplugin_getPluginAPIVersion()
{
	return PLUGIN_API_VERSION;
}

ScPlugin* newfromtemplateplugin_getPlugin()
{
	NewFromTemplatePlugin* plug = new NewFromTemplatePlugin();
	Q_CHECK_PTR(plug);
	return plug;
}

void newfromtemplateplugin_freePlugin(ScPlugin* plugin)
{
	NewFromTemplatePlugin* plug = dynamic_cast<NewFromTemplatePlugin*>(plugin);
	Q_ASSERT(plug);
	delete plug;
}

NewFromTemplatePlugin::NewFromTemplatePlugin() : ScActionPlugin()
{
	// Set action info in languageChange, so we only have to do
	// it in one place.
	languageChange();
}

NewFromTemplatePlugin::~NewFromTemplatePlugin() {};

void NewFromTemplatePlugin::languageChange()
{
	// Note that we leave the unused members unset. They'll be initialised
	// with their default ctors during construction.
	// Action name
	m_actionInfo.name = "NewFromDocumentTemplate";
	// Action text for menu, including accel
	m_actionInfo.text = tr("New &from Template...");
	// Shortcut
	m_actionInfo.keySequence = "Ctrl+Alt+N";
	// Menu
	m_actionInfo.menu = "File";
	m_actionInfo.menuAfterName = "New";
	m_actionInfo.enabledOnStartup = true;
}

const QString NewFromTemplatePlugin::fullTrName() const
{
	return QObject::tr("New From Template");
}

const ScActionPlugin::AboutData* NewFromTemplatePlugin::getAboutData() const
{
	AboutData* about = new AboutData;
	Q_CHECK_PTR(about);
	about->authors = QString::fromUtf8("Riku Leino <riku@scribus.info>");
	about->shortDescription = tr("Load documents with predefined layout");
	about->description = tr("Start a document from a template made by other users or "
	                        "yourself (f.e. for documents you have a constant style).");
    // about->version
    // about->releaseDate
    // about->copyright
	about->license = "GPL";
	return about;
}

void NewFromTemplatePlugin::deleteAboutData(const AboutData* about) const
{
	Q_ASSERT(about);
	delete about;
}

bool NewFromTemplatePlugin::run(QString target)
{
	Q_ASSERT(target.isNull());
	Nft = new MenuNFT(ScMW);
	Q_CHECK_PTR(Nft);
	Nft->RunNFTPlug();
	return true;
}


void MenuNFT::RunNFTPlug()
{
	nftdialog* nftdia = new nftdialog(ScMW, ScMW->getGuiLanguage(), PrefsManager::instance()->appPrefs.documentTemplatesDir);
	if (nftdia->exec())
	{
		qApp->setOverrideCursor(QCursor(Qt::WaitCursor), true);
		ScMW->loadDoc(QDir::cleanDirPath(nftdia->currentDocumentTemplate->file));
		ScMW->doc->hasName = false;
		UndoManager::instance()->renameStack(nftdia->currentDocumentTemplate->name);
		ScMW->doc->DocName = nftdia->currentDocumentTemplate->name;
		ScMW->updateActiveWindowCaption(QObject::tr("Document Template: ") + nftdia->currentDocumentTemplate->name);
		QDir::setCurrent(PrefsManager::instance()->documentDir());
		ScMW->removeRecent(QDir::cleanDirPath(nftdia->currentDocumentTemplate->file));
		qApp->restoreOverrideCursor();
	}
	delete nftdia;
}
